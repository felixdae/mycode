#ifndef SERVER_HANDSHAKE_H
#define SERVER_HANDSHAKE_H 
#include <map>
#include <vector>
#include <string>

namespace comm
{
    namespace websocket
    {
        class ServerHandshake
        {
            public:
                ServerHandshake();
                virtual ~ServerHandshake();
                int32_t ParseRequest(const std::string& req, std::map<int32_t, std::string>& headers);
                int32_t MakeResponse(const std::map<int32_t, std::string>& req_headers, 
                        const std::string& accept_protocol, const std::vector<std::string>& accept_extensions,
                        const std::string& uri, std::string& resp);

                static std::string NegotiateProtocol(const std::vector<std::string> &protocols, const std::map<int32_t, std::string>& headers);
                static std::vector<std::string> NegotiateExtension(const std::vector<std::string> &extensions, const std::map<int32_t, std::string>& headers);
                static bool CheckGetUri(const std::string& uri, const std::map<int32_t, std::string>& headers);

                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }
                
                enum lws_token_indexes 
                {
                    WSI_TOKEN_GET_URI,
                    WSI_TOKEN_HOST,
                    WSI_TOKEN_CONNECTION,
                    WSI_TOKEN_KEY1,
                    WSI_TOKEN_KEY2,
                    WSI_TOKEN_PROTOCOL,
                    WSI_TOKEN_UPGRADE,
                    WSI_TOKEN_ORIGIN,
                    WSI_TOKEN_DRAFT,
                    WSI_TOKEN_CHALLENGE,

                    /* new for 04 */
                    WSI_TOKEN_KEY,
                    WSI_TOKEN_VERSION,
                    WSI_TOKEN_SWORIGIN,

                    /* new for 05 */
                    WSI_TOKEN_EXTENSIONS,

                    /* client receives these */
                    WSI_TOKEN_ACCEPT,
                    WSI_TOKEN_NONCE,
                    WSI_TOKEN_HTTP,
                    WSI_TOKEN_MUXURL,

                    /* use token storage to stash these */

                    _WSI_TOKEN_CLIENT_SENT_PROTOCOLS,
                    _WSI_TOKEN_CLIENT_PEER_ADDRESS,
                    _WSI_TOKEN_CLIENT_URI,
                    _WSI_TOKEN_CLIENT_HOST,
                    _WSI_TOKEN_CLIENT_ORIGIN,

                    /* always last real token index*/
                    WSI_TOKEN_COUNT,
                    /* parser state additions */
                    WSI_TOKEN_NAME_PART,
                    WSI_TOKEN_SKIPPING,
                    WSI_TOKEN_SKIPPING_SAW_CR,
                    WSI_PARSING_COMPLETE,
                    WSI_INIT_TOKEN_MUXURL,
                };

            private:
                const static uint32_t MAX_WEBSOCKET_04_KEY_LEN = 128;
                const static int LWS_MAX_HEADER_LEN = 1024;
                struct lws_fragments {
                    unsigned short offset;
                    unsigned short len;
                    unsigned char next_frag_index;
                };

                struct allocated_headers {
                    unsigned short next_frag_index;
                    unsigned short pos;
                    unsigned char frag_index[WSI_TOKEN_COUNT];
                    struct lws_fragments frags[WSI_TOKEN_COUNT * 2];
                    char data[LWS_MAX_HEADER_LEN];
#ifndef LWS_NO_CLIENT
                    char initial_handshake_hash_base64[30];
                    unsigned short c_port;
#endif
                };

                struct _lws_header_related {
                    struct allocated_headers *ah;
                    short lextable_pos;
                    unsigned char parser_state; /* enum lws_token_indexes */
                };
            private:
                int lextable_decode(int pos, char c);
                int lws_allocate_header_table(struct _lws_header_related *hdr);
                int lws_hdr_total_length(struct _lws_header_related *hdr, enum lws_token_indexes h);
                int lws_hdr_copy(struct _lws_header_related *hdr, char *dest, int len,
                        enum lws_token_indexes h);
                char *lws_hdr_simple_ptr(struct _lws_header_related *hdr, enum lws_token_indexes h);
                int lws_hdr_simple_create(struct _lws_header_related *hdr,
                        enum lws_token_indexes h, const char *s);
                int libwebsocket_parse(struct _lws_header_related *hdr, unsigned char c);

                bool InitForParse();
            private:
                static unsigned char lextable[];

                struct _lws_header_related *hdr;
                std::string error_msg_;
//                std::vector<std::string> available_protocols_;
//                std::vector<std::string> available_extensions_;
//                std::string uri_;
        };
    }
}
#endif /* SERVER_HANDSHAKE_H */
