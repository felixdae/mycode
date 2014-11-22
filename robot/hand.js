function hand(cards){
    var self=this;
    
    self.raw_cards = cards;

    self.CONST_CARD_TYPE_HIGH_CARD = 1;
    self.CONST_CARD_TYPE_ONE_PAIR = 2;
    self.CONST_CARD_TYPE_TWO_PAIR = 3;
    self.CONST_CARD_TYPE_THREE_OF_A_KIND = 4;
    self.CONST_CARD_TYPE_STRAIGHT = 5;
    self.CONST_CARD_TYPE_FLUSH = 6;
    self.CONST_CARD_TYPE_FULL_HOUSE = 7;
    self.CONST_CARD_TYPE_FOUR_OF_A_KIND = 8;
    self.CONST_CARD_TYPE_STRAIGHT_FLUSH = 9;

    self.utility = require('./utility');
    self.dock_cards = [];
    self.gen_dock_cards = function(c){
        if (c == undefined){
            c = false;
        }
        var suite = ['s', 'h', 'c', 'd'];
        var number = [
            'a','2','3','4',
            '5','6','7','8',
            '9','10','j','q',
            'k'];
        if (c){
            suite.forEach(function(item){
                number.forEach(function(ele){
                    self.dock_cards.push(item + ele);
                });
            });
        } else {
            suite.forEach(function(item) {
                for(var i = 1; i <= 13; ++i){
                    self.dock_cards.push(item + i);
                };
            });
        }
    }

    self.check_cards = function(){
        if (self.raw_cards.length < 1 || self.raw_cards.length > 7){
            return false;
        }

        self.gen_dock_cards(true);
        //console.log(self.dock_cards);
        var il = self.raw_cards.filter(function(item){
            return self.dock_cards.indexOf(item) == -1;
        });
        if (il.length > 0){
            return false;
        }
        var my_raw_cards = utility.clone(self.raw_cards);
        my_raw_cards.sort();
        for (var i = 1; i < my_raw_cards.length; ++i){
            if (self.raw_cards[i-1] == self.raw_cards[i]){
                return false;
            }
        }
        return true;
    }

    if (true !== self.check_cards()){
        throw 'illegal hand cards: ' + JSON.stringify(self.raw_cards);
    }

    self.number_value(card){
        var number_map = {
            'a' = 14,
            'j' = 11,
            'q' = 12,
            'k' = 13,
        };
        var n = 0;
        if (card.slice(1) == 'a'
                ||card.slice(1) == 'j'
                ||card.slice(1) == 'q'
                ||card.slice(1) == 'k'){
            n = number_map[card.slice(1)];
        }else{
            n = parseInt(card.slice(1));
        }
        return n;
    }

    self.suite_value(card){
        var suite_map = {
            's' = 4,
            'h' = 3,
            'c' = 2,
            'd' = 1,
        };
        return suite_map[card[0]];
    }

    self.sort_cards = function(cards, suite_first){
        //var cards = utility.clone(self.raw_cards);
        var my_cards = utility.clone(cards);
        var eval_value = function(card){
            var n = number_value(card);
            if (suite_first){
                return self.suite_value(card) * 14 + n;
            }
            return self.suite_value(card) + n * 4;
        }
        my_cards.sort(function(a, b){
            return eval_value(a) < eval_value(b);
        });
    }

    self.detect_flush(sort_by_suite){//5
        if (self.suite_value(sort_by_suite[0]) == self.suite_value(sort_by_suite[1])
                &&self.suite_value(sort_by_suite[1]) == self.suite_value(sort_by_suite[2])
                &&self.suite_value(sort_by_suite[2]) == self.suite_value(sort_by_suite[3])
                &&self.suite_value(sort_by_suite[3]) == self.suite_value(sort_by_suite[4])){
            return true;
        }
        return false;
    }

    self.detect_straight(sort_by_number){//5
        if (self.number_value(sort_by_number[0]) + 1 == self.number_value(sort_by_number[1])
                &&self.number_value(sort_by_number[1]) + 1 == self.number_value(sort_by_number[2])
                &&self.number_value(sort_by_number[2]) + 1 == self.number_value(sort_by_number[3])
                &&self.number_value(sort_by_number[3]) + 1 == self.number_value(sort_by_number[4])){
            return true;
        }
        if (self.number_value(sort_by_number[0]) == 14 && self.number_value(sort_by_number[1]) == 2
                &&self.number_value(sort_by_number[1]) + 1 == self.number_value(sort_by_number[2])
                &&self.number_value(sort_by_number[2]) + 1 == self.number_value(sort_by_number[3])
                &&self.number_value(sort_by_number[3]) + 1 == self.number_value(sort_by_number[4])){
            return true;
        }
        return false;
    }

    self.detect_straight_flush(sort_by_suite, sort_by_number){//5
        if (self.detect_straight(sort_by_number) && self.detect_flush(sort_by_suite))
        {
            return true;
        }
        return false;
    }

    self.detect_full_house(sort_by_number){//5
        if (self.number_value(sort_by_number[0]) == self.number_value(sort_by_number[1])
                &&self.number_value(sort_by_number[1]) == self.number_value(sort_by_number[2])
                &&self.number_value(sort_by_number[3]) == self.number_value(sort_by_number[4])){
            return true;
        }
        if (self.number_value(sort_by_number[0]) == self.number_value(sort_by_number[1])
                &&self.number_value(sort_by_number[2]) == self.number_value(sort_by_number[3])
                &&self.number_value(sort_by_number[3]) == self.number_value(sort_by_number[4])){
            return true;
        }
        return false;
    }

    self.detect_four_of_a_kind(sort_by_number){//4
        if (self.number_value(sort_by_number[0]) == self.number_value(sort_by_number[1])
                &&self.number_value(sort_by_number[1]) == self.number_value(sort_by_number[2])
                &&self.number_value(sort_by_number[2]) == self.number_value(sort_by_number[3])){
            return true;
        }
        return false;
    }

    self.detect_three_of_a_kind(sort_by_number){//3
        if (self.number_value(sort_by_number[0]) == self.number_value(sort_by_number[1])
                &&self.number_value(sort_by_number[1]) == self.number_value(sort_by_number[2])){
            return true;
        }
        return false;
    }

    self.detect_two_pairs(sort_by_number){//4
        if (self.number_value(sort_by_number[0]) == self.number_value(sort_by_number[1])
                &&self.number_value(sort_by_number[2]) == self.number_value(sort_by_number[3])){
            return true;
        }
        return false;
    }
    
    self.detect_one_pair(sort_by_number){//2
        if (self.number_value(sort_by_number[0]) == self.number_value(sort_by_number[1])){
            return true;
        }
        return false;
    }

    var sort_by_suite = self.sort_cards(true, hand);
    var sort_by_number = self.sort_cards(false, hand);

    self.detect_hand(sort_by_suite, sort_by_number){//<=5 cards
        if (sort_by_number != undefined && sort_by_suite.length != sort_by_number.length){
            return false;
        }
        var hand_length = sort_by_suite.length;

        if (hand_length == 5)
        {
            if (self.detect_straight_flush(sort_by_suite, sort_by_number)){
                return self.CONST_CARD_TYPE_STRAIGHT_FLUSH;
            }
            if (self.detect_full_house(sort_by_number)){
                return self.CONST_CARD_TYPE_FULL_HOUSE;
            }
            if (self.detect_flush(sort_by_suite)){
                return self.CONST_CARD_TYPE_FLUSH;
            }
            if (self.detect_straight(sort_by_number)){
                return self.CONST_CARD_TYPE_STRAIGHT;
            }
            var type = 0;
            var max_type = self.detect_hand(sort_by_number.slice(0,4));

            type = self.detect_hand(sort_by_number.slice(1,4));
            if (type > max_type){
                max_type = type;
            }
            type = self.detect_hand(sort_by_number.slice(0,1).concat(sort_by_number.slice(2,3)));
            if (type > max_type){
                max_type = type;
            }
            type = self.detect_hand(sort_by_number.slice(0,3).concat(sort_by_number.slice(4,1)));
            if (type > max_type){
                max_type = type;
            }
            type = self.detect_hand(sort_by_number.slice(0,2).concat(sort_by_number.slice(3,2)));
            if (type > max_type){
                max_type = type;
            }
            return max_type;
        }
        if (hand_length == 4){
            if (self.detect_four_of_a_kind(sort_by_number)){
                return self.CONST_CARD_TYPE_FOUR_OF_A_KIND;
            }
            if (self.detect_two_pairs(sort_by_number){
                return self.CONST_CARD_TYPE_TWO_PAIR;
            }
            var type = 0;
            var max_type = self.detect_hand(sort_by_number.slice(0,3));

            type = self.detect_hand(sort_by_number.slice(1,3));
            if (type > max_type){
                max_type = type;
            }
            type = self.detect_hand(sort_by_number.slice(0,1).concat(sort_by_number.slice(2,2)));
            if (type > max_type){
                max_type = type;
            }
            type = self.detect_hand(sort_by_number.slice(0,2).concat(sort_by_number.slice(3,1)));
            if (type > max_type){
                max_type = type;
            }
            return max_type;
        }
        if (hand_length == 3){
            if (self.detect_three_of_a_kind(sort_by_number)){
                return self.CONST_CARD_TYPE_THREE_OF_A_KIND;
            }
            var type = 0;
            var max_type = self.detect_hand(sort_by_number.slice(0,2));

            type = self.detect_hand(sort_by_number.slice(1,2));
            if (type > max_type){
                max_type = type;
            }
            type = self.detect_hand(sort_by_number.slice(0,1).concat(sort_by_number.slice(2,1)));
            if (type > max_type){
                max_type = type;
            }
            return max_type;
        }
        if (hand_length == 2){
            if (self.detect_one_pair(sort_by_number)){
                return self.CONST_CARD_TYPE_ONE_PAIR;
            }
            return self.CONST_CARD_TYPE_HIGH_CARD;
        }
        return self.CONST_CARD_TYPE_HIGH_CARD;
    }
}

var u = new hand(['s2','s3','s2']);
//u.check_cards(true);
