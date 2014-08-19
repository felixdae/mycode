#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "server_handshake.h"
#include "libstringhelper.h"
#include "libcrypto.h"

namespace comm
{
    namespace websocket
    {
        unsigned char ServerHandshake::lextable[] = {
            /* pos 0: state 0 */
            0x47 /* 'G' */, 0x07 /* to pos 14 state 1 */,
            0x48 /* 'H' */, 0x0A /* to pos 22 state 5 */,
            0x43 /* 'C' */, 0x0F /* to pos 34 state 10 */,
            0x53 /* 'S' */, 0x19 /* to pos 56 state 21 */,
            0x55 /* 'U' */, 0x3F /* to pos 134 state 51 */,
            0x4F /* 'O' */, 0x46 /* to pos 150 state 59 */,
            0x8D /* '.' */, 0x52 /* to pos 176 state 72 */,
            /* pos 14: state 1 */
            0xC5 /* 'E' */, 0x01 /* to pos 16 state 2 */,
            /* pos 16: state 2 */
            0xD4 /* 'T' */, 0x01 /* to pos 18 state 3 */,
            /* pos 18: state 3 */
            0xA0 /* ' ' */, 0x01 /* to pos 20 state 4 */,
            /* pos 20: state 4 */
            0x80, 0x00 /* terminal marker */,
            /* pos 22: state 5 */
            0x6F /* 'o' */, 0x02 /* to pos 26 state 6 */,
            0xD4 /* 'T' */, 0x76 /* to pos 260 state 114 */,
            /* pos 26: state 6 */
            0xF3 /* 's' */, 0x01 /* to pos 28 state 7 */,
            /* pos 28: state 7 */
            0xF4 /* 't' */, 0x01 /* to pos 30 state 8 */,
            /* pos 30: state 8 */
            0xBA /* ':' */, 0x01 /* to pos 32 state 9 */,
            /* pos 32: state 9 */
            0x81, 0x00 /* terminal marker */,
            /* pos 34: state 10 */
            0xEF /* 'o' */, 0x01 /* to pos 36 state 11 */,
            /* pos 36: state 11 */
            0xEE /* 'n' */, 0x01 /* to pos 38 state 12 */,
            /* pos 38: state 12 */
            0xEE /* 'n' */, 0x01 /* to pos 40 state 13 */,
            /* pos 40: state 13 */
            0xE5 /* 'e' */, 0x01 /* to pos 42 state 14 */,
            /* pos 42: state 14 */
            0xE3 /* 'c' */, 0x01 /* to pos 44 state 15 */,
            /* pos 44: state 15 */
            0xF4 /* 't' */, 0x01 /* to pos 46 state 16 */,
            /* pos 46: state 16 */
            0xE9 /* 'i' */, 0x01 /* to pos 48 state 17 */,
            /* pos 48: state 17 */
            0xEF /* 'o' */, 0x01 /* to pos 50 state 18 */,
            /* pos 50: state 18 */
            0xEE /* 'n' */, 0x01 /* to pos 52 state 19 */,
            /* pos 52: state 19 */
            0xBA /* ':' */, 0x01 /* to pos 54 state 20 */,
            /* pos 54: state 20 */
            0x82, 0x00 /* terminal marker */,
            /* pos 56: state 21 */
            0xE5 /* 'e' */, 0x01 /* to pos 58 state 22 */,
            /* pos 58: state 22 */
            0xE3 /* 'c' */, 0x01 /* to pos 60 state 23 */,
            /* pos 60: state 23 */
            0xAD /* '-' */, 0x01 /* to pos 62 state 24 */,
            /* pos 62: state 24 */
            0xD7 /* 'W' */, 0x01 /* to pos 64 state 25 */,
            /* pos 64: state 25 */
            0xE5 /* 'e' */, 0x01 /* to pos 66 state 26 */,
            /* pos 66: state 26 */
            0xE2 /* 'b' */, 0x01 /* to pos 68 state 27 */,
            /* pos 68: state 27 */
            0xD3 /* 'S' */, 0x01 /* to pos 70 state 28 */,
            /* pos 70: state 28 */
            0xEF /* 'o' */, 0x01 /* to pos 72 state 29 */,
            /* pos 72: state 29 */
            0xE3 /* 'c' */, 0x01 /* to pos 74 state 30 */,
            /* pos 74: state 30 */
            0xEB /* 'k' */, 0x01 /* to pos 76 state 31 */,
            /* pos 76: state 31 */
            0xE5 /* 'e' */, 0x01 /* to pos 78 state 32 */,
            /* pos 78: state 32 */
            0xF4 /* 't' */, 0x01 /* to pos 80 state 33 */,
            /* pos 80: state 33 */
            0xAD /* '-' */, 0x01 /* to pos 82 state 34 */,
            /* pos 82: state 34 */
            0x4B /* 'K' */, 0x08 /* to pos 98 state 35 */,
            0x50 /* 'P' */, 0x10 /* to pos 116 state 42 */,
            0x44 /* 'D' */, 0x27 /* to pos 164 state 66 */,
            0x56 /* 'V' */, 0x2F /* to pos 182 state 75 */,
            0x4F /* 'O' */, 0x36 /* to pos 198 state 83 */,
            0x45 /* 'E' */, 0x3C /* to pos 212 state 90 */,
            0x41 /* 'A' */, 0x46 /* to pos 234 state 101 */,
            0xCE /* 'N' */, 0x4C /* to pos 248 state 108 */,
            /* pos 98: state 35 */
            0xE5 /* 'e' */, 0x01 /* to pos 100 state 36 */,
            /* pos 100: state 36 */
            0xF9 /* 'y' */, 0x01 /* to pos 102 state 37 */,
            /* pos 102: state 37 */
            0x31 /* '1' */, 0x03 /* to pos 108 state 38 */,
            0x32 /* '2' */, 0x04 /* to pos 112 state 40 */,
            0xBA /* ':' */, 0x25 /* to pos 180 state 74 */,
            /* pos 108: state 38 */
            0xBA /* ':' */, 0x01 /* to pos 110 state 39 */,
            /* pos 110: state 39 */
            0x83, 0x00 /* terminal marker */,
            /* pos 112: state 40 */
            0xBA /* ':' */, 0x01 /* to pos 114 state 41 */,
            /* pos 114: state 41 */
            0x84, 0x00 /* terminal marker */,
            /* pos 116: state 42 */
            0xF2 /* 'r' */, 0x01 /* to pos 118 state 43 */,
            /* pos 118: state 43 */
            0xEF /* 'o' */, 0x01 /* to pos 120 state 44 */,
            /* pos 120: state 44 */
            0xF4 /* 't' */, 0x01 /* to pos 122 state 45 */,
            /* pos 122: state 45 */
            0xEF /* 'o' */, 0x01 /* to pos 124 state 46 */,
            /* pos 124: state 46 */
            0xE3 /* 'c' */, 0x01 /* to pos 126 state 47 */,
            /* pos 126: state 47 */
            0xEF /* 'o' */, 0x01 /* to pos 128 state 48 */,
            /* pos 128: state 48 */
            0xEC /* 'l' */, 0x01 /* to pos 130 state 49 */,
            /* pos 130: state 49 */
            0xBA /* ':' */, 0x01 /* to pos 132 state 50 */,
            /* pos 132: state 50 */
            0x85, 0x00 /* terminal marker */,
            /* pos 134: state 51 */
            0xF0 /* 'p' */, 0x01 /* to pos 136 state 52 */,
            /* pos 136: state 52 */
            0xE7 /* 'g' */, 0x01 /* to pos 138 state 53 */,
            /* pos 138: state 53 */
            0xF2 /* 'r' */, 0x01 /* to pos 140 state 54 */,
            /* pos 140: state 54 */
            0xE1 /* 'a' */, 0x01 /* to pos 142 state 55 */,
            /* pos 142: state 55 */
            0xE4 /* 'd' */, 0x01 /* to pos 144 state 56 */,
            /* pos 144: state 56 */
            0xE5 /* 'e' */, 0x01 /* to pos 146 state 57 */,
            /* pos 146: state 57 */
            0xBA /* ':' */, 0x01 /* to pos 148 state 58 */,
            /* pos 148: state 58 */
            0x86, 0x00 /* terminal marker */,
            /* pos 150: state 59 */
            0xF2 /* 'r' */, 0x01 /* to pos 152 state 60 */,
            /* pos 152: state 60 */
            0xE9 /* 'i' */, 0x01 /* to pos 154 state 61 */,
            /* pos 154: state 61 */
            0xE7 /* 'g' */, 0x01 /* to pos 156 state 62 */,
            /* pos 156: state 62 */
            0xE9 /* 'i' */, 0x01 /* to pos 158 state 63 */,
            /* pos 158: state 63 */
            0xEE /* 'n' */, 0x01 /* to pos 160 state 64 */,
            /* pos 160: state 64 */
            0xBA /* ':' */, 0x01 /* to pos 162 state 65 */,
            /* pos 162: state 65 */
            0x87, 0x00 /* terminal marker */,
            /* pos 164: state 66 */
            0xF2 /* 'r' */, 0x01 /* to pos 166 state 67 */,
            /* pos 166: state 67 */
            0xE1 /* 'a' */, 0x01 /* to pos 168 state 68 */,
            /* pos 168: state 68 */
            0xE6 /* 'f' */, 0x01 /* to pos 170 state 69 */,
            /* pos 170: state 69 */
            0xF4 /* 't' */, 0x01 /* to pos 172 state 70 */,
            /* pos 172: state 70 */
            0xBA /* ':' */, 0x01 /* to pos 174 state 71 */,
            /* pos 174: state 71 */
            0x88, 0x00 /* terminal marker */,
            /* pos 176: state 72 */
            0x8A /* '.' */, 0x01 /* to pos 178 state 73 */,
            /* pos 178: state 73 */
            0x89, 0x00 /* terminal marker */,
            /* pos 180: state 74 */
            0x8A, 0x00 /* terminal marker */,
            /* pos 182: state 75 */
            0xE5 /* 'e' */, 0x01 /* to pos 184 state 76 */,
            /* pos 184: state 76 */
            0xF2 /* 'r' */, 0x01 /* to pos 186 state 77 */,
            /* pos 186: state 77 */
            0xF3 /* 's' */, 0x01 /* to pos 188 state 78 */,
            /* pos 188: state 78 */
            0xE9 /* 'i' */, 0x01 /* to pos 190 state 79 */,
            /* pos 190: state 79 */
            0xEF /* 'o' */, 0x01 /* to pos 192 state 80 */,
            /* pos 192: state 80 */
            0xEE /* 'n' */, 0x01 /* to pos 194 state 81 */,
            /* pos 194: state 81 */
            0xBA /* ':' */, 0x01 /* to pos 196 state 82 */,
            /* pos 196: state 82 */
            0x8B, 0x00 /* terminal marker */,
            /* pos 198: state 83 */
            0xF2 /* 'r' */, 0x01 /* to pos 200 state 84 */,
            /* pos 200: state 84 */
            0xE9 /* 'i' */, 0x01 /* to pos 202 state 85 */,
            /* pos 202: state 85 */
            0xE7 /* 'g' */, 0x01 /* to pos 204 state 86 */,
            /* pos 204: state 86 */
            0xE9 /* 'i' */, 0x01 /* to pos 206 state 87 */,
            /* pos 206: state 87 */
            0xEE /* 'n' */, 0x01 /* to pos 208 state 88 */,
            /* pos 208: state 88 */
            0xBA /* ':' */, 0x01 /* to pos 210 state 89 */,
            /* pos 210: state 89 */
            0x8C, 0x00 /* terminal marker */,
            /* pos 212: state 90 */
            0xF8 /* 'x' */, 0x01 /* to pos 214 state 91 */,
            /* pos 214: state 91 */
            0xF4 /* 't' */, 0x01 /* to pos 216 state 92 */,
            /* pos 216: state 92 */
            0xE5 /* 'e' */, 0x01 /* to pos 218 state 93 */,
            /* pos 218: state 93 */
            0xEE /* 'n' */, 0x01 /* to pos 220 state 94 */,
            /* pos 220: state 94 */
            0xF3 /* 's' */, 0x01 /* to pos 222 state 95 */,
            /* pos 222: state 95 */
            0xE9 /* 'i' */, 0x01 /* to pos 224 state 96 */,
            /* pos 224: state 96 */
            0xEF /* 'o' */, 0x01 /* to pos 226 state 97 */,
            /* pos 226: state 97 */
            0xEE /* 'n' */, 0x01 /* to pos 228 state 98 */,
            /* pos 228: state 98 */
            0xF3 /* 's' */, 0x01 /* to pos 230 state 99 */,
            /* pos 230: state 99 */
            0xBA /* ':' */, 0x01 /* to pos 232 state 100 */,
            /* pos 232: state 100 */
            0x8D, 0x00 /* terminal marker */,
            /* pos 234: state 101 */
            0xE3 /* 'c' */, 0x01 /* to pos 236 state 102 */,
            /* pos 236: state 102 */
            0xE3 /* 'c' */, 0x01 /* to pos 238 state 103 */,
            /* pos 238: state 103 */
            0xE5 /* 'e' */, 0x01 /* to pos 240 state 104 */,
            /* pos 240: state 104 */
            0xF0 /* 'p' */, 0x01 /* to pos 242 state 105 */,
            /* pos 242: state 105 */
            0xF4 /* 't' */, 0x01 /* to pos 244 state 106 */,
            /* pos 244: state 106 */
            0xBA /* ':' */, 0x01 /* to pos 246 state 107 */,
            /* pos 246: state 107 */
            0x8E, 0x00 /* terminal marker */,
            /* pos 248: state 108 */
            0xEF /* 'o' */, 0x01 /* to pos 250 state 109 */,
            /* pos 250: state 109 */
            0xEE /* 'n' */, 0x01 /* to pos 252 state 110 */,
            /* pos 252: state 110 */
            0xE3 /* 'c' */, 0x01 /* to pos 254 state 111 */,
            /* pos 254: state 111 */
            0xE5 /* 'e' */, 0x01 /* to pos 256 state 112 */,
            /* pos 256: state 112 */
            0xBA /* ':' */, 0x01 /* to pos 258 state 113 */,
            /* pos 258: state 113 */
            0x8F, 0x00 /* terminal marker */,
            /* pos 260: state 114 */
            0xD4 /* 'T' */, 0x01 /* to pos 262 state 115 */,
            /* pos 262: state 115 */
            0xD0 /* 'P' */, 0x01 /* to pos 264 state 116 */,
            /* pos 264: state 116 */
            0xAF /* '/' */, 0x01 /* to pos 266 state 117 */,
            /* pos 266: state 117 */
            0xB1 /* '1' */, 0x01 /* to pos 268 state 118 */,
            /* pos 268: state 118 */
            0xAE /* '.' */, 0x01 /* to pos 270 state 119 */,
            /* pos 270: state 119 */
            0xB1 /* '1' */, 0x01 /* to pos 272 state 120 */,
            /* pos 272: state 120 */
            0xA0 /* ' ' */, 0x01 /* to pos 274 state 121 */,
            /* pos 274: state 121 */
            0x90, 0x00 /* terminal marker */,
            /* total size 276 bytes */
        };

        int ServerHandshake::lextable_decode(int pos, char c)
        {
            while (pos >= 0) {
                if (lextable[pos + 1] == 0) /* terminal marker */
                    return pos;

                if ((lextable[pos] & 0x7f) == c)
                    return pos + (lextable[pos + 1] << 1);

                if (lextable[pos] & 0x80)
                    return -1;

                pos += 2;
            }
            return pos;
        }

        int ServerHandshake::lws_allocate_header_table(struct _lws_header_related *hdr)
        {
            hdr->ah = (struct allocated_headers *)malloc(sizeof(*hdr->ah));
            if (hdr->ah == NULL) {
                //fprintf(stdout, "Out of memory\n");
                return -1;
            }
            memset(hdr->ah->frag_index, 0, sizeof(hdr->ah->frag_index));
            hdr->ah->next_frag_index = 0;
            hdr->ah->pos = 0;

            return 0;
        }

        int ServerHandshake::lws_hdr_total_length(struct _lws_header_related *hdr, enum lws_token_indexes h)
        {
            int n;
            int len = 0;

            n = hdr->ah->frag_index[h];
            if (n == 0)
                return 0;

            do {
                len += hdr->ah->frags[n].len;
                n = hdr->ah->frags[n].next_frag_index;
            } while (n);

            return len;
        }

        int ServerHandshake::lws_hdr_copy(struct _lws_header_related *hdr, char *dest, int len,
                enum lws_token_indexes h)
        {
            int toklen = lws_hdr_total_length(hdr, h);
            int n;

            if (toklen >= len)
                return -1;

            n = hdr->ah->frag_index[h];
            if (n == 0)
                return 0;

            do {
                strcpy(dest,
                        &hdr->ah->data[hdr->ah->frags[n].offset]);
                dest += hdr->ah->frags[n].len;
                n = hdr->ah->frags[n].next_frag_index;
            } while (n);

            return toklen;
        }

        char* ServerHandshake::lws_hdr_simple_ptr(struct _lws_header_related *hdr, enum lws_token_indexes h)
        {
            int n;

            n = hdr->ah->frag_index[h];
            if (!n)
                return NULL;

            return &hdr->ah->data[hdr->ah->frags[n].offset];
        }

        int ServerHandshake::lws_hdr_simple_create(struct _lws_header_related *hdr,
                enum lws_token_indexes h, const char *s)
        {
            hdr->ah->next_frag_index++;
            if (hdr->ah->next_frag_index ==
                    sizeof(hdr->ah->frags) / sizeof(hdr->ah->frags[0])) {
                //fprintf(stdout, "More hdr frags than we can deal with, dropping\n");
                return -1;
            }

            hdr->ah->frag_index[h] = hdr->ah->next_frag_index;

            hdr->ah->frags[hdr->ah->next_frag_index].offset =
                hdr->ah->pos;
            hdr->ah->frags[hdr->ah->next_frag_index].len = 0;
            hdr->ah->frags[hdr->ah->next_frag_index].next_frag_index =
                0;

            do {
                if (hdr->ah->pos == sizeof(hdr->ah->data)) {
                    //fprintf(stdout, "Ran out of header data space\n");
                    return -1;
                }
                hdr->ah->data[hdr->ah->pos++] = *s;
                if (*s)
                    hdr->ah->frags[
                        hdr->ah->next_frag_index].len++;
            } while (*s++);

            return 0;
        }

        int ServerHandshake::libwebsocket_parse(struct _lws_header_related *hdr, unsigned char c)
        {
            int n;

            switch (hdr->parser_state) {
                case WSI_TOKEN_GET_URI:
                case WSI_TOKEN_HOST:
                case WSI_TOKEN_CONNECTION:
                case WSI_TOKEN_KEY1:
                case WSI_TOKEN_KEY2:
                case WSI_TOKEN_PROTOCOL:
                case WSI_TOKEN_UPGRADE:
                case WSI_TOKEN_ORIGIN:
                case WSI_TOKEN_SWORIGIN:
                case WSI_TOKEN_DRAFT:
                case WSI_TOKEN_CHALLENGE:
                case WSI_TOKEN_KEY:
                case WSI_TOKEN_VERSION:
                case WSI_TOKEN_ACCEPT:
                case WSI_TOKEN_NONCE:
                case WSI_TOKEN_EXTENSIONS:
                case WSI_TOKEN_HTTP:

                    //fprintf(stdout, "WSI_TOK_(%d) '%c'\n", hdr->parser_state, c);

                    /* collect into malloc'd buffers */
                    /* optional initial space swallow */
                    if (!hdr->ah->frags[hdr->ah->frag_index[
                            hdr->parser_state]].len && c == ' ')
                        break;

                    /* special case space terminator for get-uri */
                    if (hdr->parser_state == WSI_TOKEN_GET_URI && c == ' ') {
                        c = '\0';
                        hdr->parser_state = WSI_TOKEN_SKIPPING;
                    }

                    /* bail at EOL */
                    if (hdr->parser_state != WSI_TOKEN_CHALLENGE &&
                            c == '\x0d') {
                        c = '\0';
                        hdr->parser_state = WSI_TOKEN_SKIPPING_SAW_CR;
                        //fprintf(stdout, "*\n");
                    }

                    if (hdr->ah->pos == sizeof(hdr->ah->data)) {
                        //fprintf(stdout, "excessive header content\n");
                        return -1;
                    }
                    hdr->ah->data[hdr->ah->pos++] = c;
                    if (c)
                        hdr->ah->frags[
                            hdr->ah->next_frag_index].len++;

                    /* per-protocol end of headers management */

                    if (hdr->parser_state == WSI_TOKEN_CHALLENGE)
                        goto set_parsing_complete;
                    break;

                    /* collecting and checking a name part */
                case WSI_TOKEN_NAME_PART:
                    //fprintf(stdout, "WSI_TOKEN_NAME_PART '%c'\n", c);

                    hdr->lextable_pos =
                        lextable_decode(hdr->lextable_pos, c);

                    if (hdr->lextable_pos < 0) {
                        /* this is not a header we know about */
                        if (hdr->ah->frag_index[WSI_TOKEN_GET_URI] ||
                                hdr->ah->frag_index[WSI_TOKEN_HTTP]) {
                            /*
                             * altready had the method, no idea what
                             * this crap is, ignore
                             */
                            hdr->parser_state = WSI_TOKEN_SKIPPING;
                            break;
                        }
                        /*
                         * hm it's an unknown http method in fact,
                         * treat as dangerous
                         */

                        //fprintf(stdout, "Unknown method - dropping\n");
                        return -1;
                    }
                    if (lextable[hdr->lextable_pos + 1] == 0) {

                        /* terminal state */

                        n = lextable[hdr->lextable_pos] & 0x7f;

                        //fprintf(stdout, "known hdr %d\n", n);

                        if (n == WSI_TOKEN_GET_URI &&
                                hdr->ah->frag_index[WSI_TOKEN_GET_URI]) {
                            //fprintf(stdout, "Duplicated GET\n");
                            return -1;
                        }

                        /*
                         * WSORIGIN is protocol equiv to ORIGIN,
                         * JWebSocket likes to send it, map to ORIGIN
                         */
                        if (n == WSI_TOKEN_SWORIGIN)
                            n = WSI_TOKEN_ORIGIN;

                        hdr->parser_state = (enum lws_token_indexes)
                            (WSI_TOKEN_GET_URI + n);
                        if (hdr->parser_state == WSI_TOKEN_CHALLENGE)
                            goto set_parsing_complete;

                        goto start_fragment;
                    }
                    break;

start_fragment:
                    hdr->ah->next_frag_index++;
                    if (hdr->ah->next_frag_index ==
                            sizeof(hdr->ah->frags) /
                            sizeof(hdr->ah->frags[0])) {
                        //fprintf(stdout, "More hdr frags than we can deal with\n");
                        return -1;
                    }

                    hdr->ah->frags[hdr->ah->next_frag_index].offset =
                        hdr->ah->pos;
                    hdr->ah->frags[hdr->ah->next_frag_index].len = 0;
                    hdr->ah->frags[
                        hdr->ah->next_frag_index].next_frag_index = 0;

                    n = hdr->ah->frag_index[hdr->parser_state];
                    if (!n) { /* first fragment */
                        hdr->ah->frag_index[hdr->parser_state] =
                            hdr->ah->next_frag_index;
                    } else { /* continuation */
                        while (hdr->ah->frags[n].next_frag_index)
                            n = hdr->ah->frags[n].next_frag_index;
                        hdr->ah->frags[n].next_frag_index =
                            hdr->ah->next_frag_index;

                        if (hdr->ah->pos == sizeof(hdr->ah->data)) {
                            //fprintf(stdout, "excessive header content\n");
                            return -1;
                        }

                        hdr->ah->data[hdr->ah->pos++] = ' ';
                        hdr->ah->frags[
                            hdr->ah->next_frag_index].len++;
                    }

                    break;


                    /* skipping arg part of a name we didn't recognize */
                case WSI_TOKEN_SKIPPING:
                    //fprintf(stdout, "WSI_TOKEN_SKIPPING '%c'\n", c);
                    if (c == '\x0d')
                        hdr->parser_state = WSI_TOKEN_SKIPPING_SAW_CR;
                    break;

                case WSI_TOKEN_SKIPPING_SAW_CR:
                    //fprintf(stdout, "WSI_TOKEN_SKIPPING_SAW_CR '%c'\n", c);
                    if (c == '\x0a') {
                        hdr->parser_state = WSI_TOKEN_NAME_PART;
                        hdr->lextable_pos = 0;
                    } else
                        hdr->parser_state = WSI_TOKEN_SKIPPING;
                    break;
                    /* we're done, ignore anything else */
                case WSI_PARSING_COMPLETE:
                    //fprintf(stdout, "WSI_PARSING_COMPLETE '%c'\n", c);
                    break;

                default:	/* keep gcc happy */
                    break;
            }

            return 0;

set_parsing_complete:

//            unsigned char ietf_spec_revision;
//            if (lws_hdr_total_length(hdr, WSI_TOKEN_UPGRADE)) {
//                if (lws_hdr_total_length(hdr, WSI_TOKEN_VERSION))
//                    ietf_spec_revision =
//                        atoi(lws_hdr_simple_ptr(hdr, WSI_TOKEN_VERSION));
//
//                //fprintf(stdout, "v%02d hdrs completed\n", ietf_spec_revision);
//            }
            hdr->parser_state = WSI_PARSING_COMPLETE;
            return 0;
        }

        bool ServerHandshake::InitForParse()
        {
            if (hdr == NULL)
            {
                hdr = (struct _lws_header_related*)malloc(sizeof(struct _lws_header_related));
                memset(hdr, 0, sizeof(struct _lws_header_related));
            }
            else
            {
                free(hdr->ah);
                memset(hdr, 0, sizeof(struct _lws_header_related));
            }
            hdr->parser_state = WSI_TOKEN_NAME_PART;
            hdr->lextable_pos = 0;
            lws_allocate_header_table(hdr);

            //when data is ready, set init state
            return true;
        }
        
        ServerHandshake::ServerHandshake()
        :hdr(NULL)
        {
        }

        ServerHandshake::~ServerHandshake()
        {
            if (hdr->ah)
            {
                free(hdr->ah);
            }
            if (hdr)
            {
                free(hdr);
            }
        }

        int32_t ServerHandshake::ParseRequest(const std::string& req, std::map<int32_t, std::string>& headers)
        {
            this->InitForParse();
            for (uint32_t i = 0; i < req.length(); ++i)
            {
                if (libwebsocket_parse(hdr, req[i]))
                {
                    this->error_msg_ = "libwebsocket_parse fail: " + req + ", at pos: " + std::to_string(i);
                    return -1;
                }
            }
            if (hdr->parser_state != WSI_PARSING_COMPLETE)
            {

                this->error_msg_ = "parse not complete";
                return -1;
            }

            char buf[2048];
            headers.clear();
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_KEY) > 0)
            {
                headers[WSI_TOKEN_KEY] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_VERSION) > 0)
            {
                headers[WSI_TOKEN_VERSION] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_SWORIGIN) > 0)
            {
                headers[WSI_TOKEN_SWORIGIN] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_EXTENSIONS) > 0)
            {
                headers[WSI_TOKEN_EXTENSIONS] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_ACCEPT) > 0)
            {
                headers[WSI_TOKEN_ACCEPT] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_NONCE) > 0)
            {
                headers[WSI_TOKEN_NONCE] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_HTTP) > 0)
            {
                headers[WSI_TOKEN_HTTP] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_MUXURL) > 0)
            {
                headers[WSI_TOKEN_MUXURL] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_GET_URI) > 0)
            {
                headers[WSI_TOKEN_GET_URI] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_HOST) > 0)
            {
                headers[WSI_TOKEN_HOST] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_CONNECTION) > 0)
            {
                headers[WSI_TOKEN_CONNECTION] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_KEY1) > 0)
            {
                headers[WSI_TOKEN_KEY1] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_KEY2) > 0)
            {
                headers[WSI_TOKEN_KEY2] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_PROTOCOL) > 0)
            {
                headers[WSI_TOKEN_PROTOCOL] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_UPGRADE) > 0)
            {
                headers[WSI_TOKEN_UPGRADE] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_ORIGIN) > 0)
            {
                headers[WSI_TOKEN_ORIGIN] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_DRAFT) > 0)
            {
                headers[WSI_TOKEN_DRAFT] = buf;
            }
            if (lws_hdr_copy(hdr, buf, sizeof(buf), WSI_TOKEN_CHALLENGE) > 0)
            {
                headers[WSI_TOKEN_CHALLENGE] = buf;
            }

            //for(auto p:req)
            //{
            //    printf("%d=>%s\n", p.first, p.second.c_str());
            //}

            //if (req.count(WSI_TOKEN_UPGRADE) == 0 or req.count(WSI_TOKEN_CONNECTION) == 0)
            //{
            //    return -1;
            //}

            //std::vector<std::string> avail_protocol = {"default-protocol"};
            //std::string accepted_protocol;
            //if(req.count(WSI_TOKEN_PROTOCOL) == 1)
            //{
            //    if (req[WSI_TOKEN_PROTOCOL] == avail_protocol[0])
            //    {
            //        accepted_protocol = req[WSI_TOKEN_PROTOCOL];
            //    }
            //    else
            //    {
            //        //unsupported  protocol
            //    }
            //}
            return 0;
        }

        std::string ServerHandshake::NegotiateProtocol(const std::vector<std::string> &protocols, const std::map<int32_t, std::string>& headers)
        {
            if (headers.count(WSI_TOKEN_PROTOCOL) == 0)
            {
                return "";
            }

            std::vector<std::string> headers_protocols(comm::library::StringHelper::Split(headers.find(WSI_TOKEN_PROTOCOL)->second, ','));
            for_each(headers_protocols.begin(), headers_protocols.end(), [](std::string& s){s = comm::library::StringHelper::Trim(s);});

            for (auto t1:protocols)
                for (auto t2:headers_protocols)
                    if (t1 == t2)
                    {
                        return t1;
                    }

            return "";
        }

        std::vector<std::string> ServerHandshake::NegotiateExtension(const std::vector<std::string> &extensions, const std::map<int32_t, std::string>& headers)
        {
            std::vector<std::string> res;
            if (headers.count(WSI_TOKEN_EXTENSIONS) == 0)
            {
                return res;
            }

            std::vector<std::string> headers_extensions(comm::library::StringHelper::Split(headers.find(WSI_TOKEN_EXTENSIONS)->second, ','));
            for_each(headers_extensions.begin(), headers_extensions.end(), [](std::string& s){s = comm::library::StringHelper::Trim(s);});

            for (auto t1:extensions)
                for (auto t2:headers_extensions)
                    if (t1 == t2)
                    {
                        res.push_back(t1);
                    }

            return res;
        }

        bool ServerHandshake::CheckGetUri(const std::string& uri, const std::map<int32_t, std::string>& headers)
        {
            if (headers.count(WSI_TOKEN_GET_URI) == 0)
                return false;
            return uri == headers.find(WSI_TOKEN_GET_URI)->second;
        }


        int32_t ServerHandshake::MakeResponse(const std::map<int32_t, std::string>& req_headers,
                const std::string& accept_protocol, const std::vector<std::string>& accept_extensions,
                const std::string& uri, std::string& resp)
        {
            resp.clear();
            if (req_headers.count(WSI_TOKEN_UPGRADE) == 0)
            {
                this->error_msg_ = "missing WSI_TOKEN_UPGRADE";
                return -1;
            }
            if (req_headers.count(WSI_TOKEN_CONNECTION) == 0)
            {
                this->error_msg_ = "missing WSI_TOKEN_CONNECTION";
                return -1;
            }
            if (req_headers.count(WSI_TOKEN_HOST) == 0)
            {
                this->error_msg_ = "missing WSI_TOKEN_HOST";
                return -1;
            }
            if (CheckGetUri(uri, req_headers) == false)
            {
                this->error_msg_ = "wrong uri: " + uri;
                return -1;
            }
            if (req_headers.count(WSI_TOKEN_KEY) == 0)
            {
                this->error_msg_ = "missing WSI_TOKEN_KEY";
                return -1;
            }
            if (req_headers.find(WSI_TOKEN_KEY)->second.length() >= MAX_WEBSOCKET_04_KEY_LEN)
            {
                this->error_msg_ = "WSI_TOKEN_KEY too long";
                return -1;
            }
            std::string to_hash = req_headers.find(WSI_TOKEN_KEY)->second + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            std::string accept = comm::library::Crypto::B64encode(comm::library::Crypto::Sha1(to_hash));

            resp = std::string("HTTP/1.1 101 Switching Protocols\x0d\x0a")
                + "Upgrade: WebSocket\x0d\x0a" 
		        + "Connection: Upgrade\x0d\x0a"
		        + "Sec-WebSocket-Accept: " + accept + "\x0d\x0a";

            //std::string accept_protocol = NegotiateProtocol(protocols, req_headers); 
            if (!accept_protocol.empty())
            {
                resp += "Sec-WebSocket-Protocol: " + accept_protocol + "\x0d\x0a";
            }
            //std::vector<std::string> accept_extensions = NegotiateExtension(extensions, req_headers);
            if (!accept_extensions.empty())
            {
                resp += "Sec-WebSocket-Extensions: " + comm::library::StringHelper::Join(accept_extensions, ',') + "\x0d\x0a";
            }

            resp += "\x0d\x0a";
            return 0;
        }
    }
}

//gcc server_handshake.cpp ../../comm/library/libcrypto.cpp ../../comm/library/libstringhelper.cpp -g -Wall -std=c++11 -I../../comm/library -lmcrypt -lcrypto -lstdc++ 
//#include <stdio.h>
//int main(int argc, char**argv)
//{
//    std::string req = "GET / HTTP/1.1\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nHost: 10.0.1.28:7777\r\nOrigin: http://10.0.1.28\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nSec-WebSocket-Key: AifRkv/pS5/YPZRpDghsMg==\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Extensions: permessage-deflate; client_max_window_bits, x-webkit-deflate-frame\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36\r\n\r\n";
//
//    comm::websocket::ServerHandshake hs;
//    std::map<int32_t, std::string> headers;
//    hs.ParseRequest(req, headers);
//
//    for(auto p:headers)
//    {
//        printf("%d=>%s\n", p.first, p.second.c_str());
//    }
//
//    std::string resp;
//    int rc = hs.MakeResponse(headers, {}/*const std::vector<std::string>*/, {"x-webkit-deflate-frame"}, "/", resp);
//    printf("\n%d\n%s", rc, resp.c_str());
//    return 0;
//}
