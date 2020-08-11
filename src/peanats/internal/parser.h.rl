#include <peanats/internal/globals.h>
#include <peanats/internal/parser.h>
#include <peanats/internal/support.h>

PEANATS_NAMESPACE_BEGIN

%%{
  machine m;
  access this->;


  action end_ping {
    log("[<<]: ");
    logn(tokens.to_full_string());
    on_parser_ping();
  }

  action end_pong {
    log("[<<]: ");
    logn(tokens.to_full_string());
    on_parser_pong();
  }

  action end_info {
    //log("[<<]: ");
    //logn(tokens.to_full_string());
    //on_parser_info(tokens.front(), tokens.back());
  }

  action end_ok {
    log("[<<]: ");
    logn(tokens.to_full_string());
    on_parser_ok();
  }

  action end_err {
    log("[<<]: ");
    logn(tokens.to_full_string());
    on_parser_err(tokens.back());
  }

  action end_msg
  {
    log("[<<]: ");
    logn(support::human_readable(tokens.to_full_string()));
    
    Message m(peanats);

    // subject is the second token
    m.subject = tokens[1];

    // sid is the third
    m.sid = std::atoi(tokens[2].data());

    // replyto may be the third or forth token depending on how many
    // tokens we have collected, otherwise the string is already empty
    if (tokens.size() >= 6)
      m.replyto = tokens[3];

    // Payload is the last stored argument and we dont need it's length
    m.payload = tokens.back();

    on_parser_msg( m );
  }

  # used to keep pointers to tokens we encounter while scanning
  action start_token    { tokens.start(p); }
  action end_token      { tokens.stop(p); }
  action clear_tokens   { tokens.clear(); }

  # Sent by server:
  # INFO, MSG, +OK, -ERR, PING, PONG

  # White space (space | tab)
  sp   = (' ' | '\t');

  # linefeed
  crlf = '\r\n';
  
  payload := 
    (
      any*
      ${
        // n = std::min(remaining_payload, pe-p)
        //size_t n = pe - p;
        //if (n > remaining_payload)
        //  n = remaining_payload;
        //
        //p += n;
        //remaining_payload -= n;

        if (remaining_payload-- == 0) {
          tokens.stop(p);
          fhold; fret;
        }
      }

      >{
        // Decode the length of the payload as it's the last token
        // that we grabbed
        remaining_payload = std::atoi(tokens.back().data());

        // Mark the start of the payload token
        tokens.start(p); 
      }
    );
  
  number  = digit+          >start_token %end_token;
  subject = (alnum | '.')+  >start_token %end_token;
  err_msg = (alnum | sp)+   >start_token %end_token;

  # includes a empty string
  double_quote = '\"';
  double_quote_string = 
    double_quote
    ^double_quote+  >start_token %end_token
    double_quote;

  # includes a empty string
  single_quote = '\'';
  single_quote_string = 
    single_quote
    ^single_quote*  >start_token %end_token
    single_quote;


  #info_member = '"' info_identifier '"' ':' info_value;

  op_err   = '-ERR' >start_token %end_token sp single_quote_string crlf @end_err;
  op_ok    = '+OK'  >start_token %end_token crlf @end_ok;
  op_ping  = 'PING' >start_token %end_token crlf @end_ping;
  op_pong  = 'PONG' >start_token %end_token crlf @end_pong;

  # skip info for now, TODO: fix
  op_info = 'INFO' >start_token sp+ '{' (any - '}')* '}' sp*
      %end_token crlf @end_info;

  # MSG <subject> <sid> [reply-to] <#bytes>\r\n[payload]\r\n
  op_msg   = 'MSG' >start_token %end_token
      sp subject sp number sp (subject sp)? number crlf
      @{fcall payload; }
      crlf @end_msg;

  operation = (op_ping | op_pong | op_info | op_ok | op_err | op_msg)
      >clear_tokens;

  main := operation+
    $err {
      debug_logn("Parser error at: " + support::human_readable(std::string(p,pe-p)));
      fgoto main;
    };

  write data;

}%%

void Parser::clear_state()
{
  %% write init;
}

inline const int Parser::first_final_state() {
  return %%{ write first_final; }%%;
}

inline void Parser::ragel_parser(Receiver& receiver)
{
  char* p = receiver._cursor;
  char* pe = receiver.data() + receiver.have();
  char* eof = 0;

  //debug_logn("Parsing: " + support::human_readable(std::string(p, pe-p)));

  %% write exec;

  // store the new cursor and the caller will do the required
  // adjustments to the buffer
  receiver._cursor = p;
}


PEANATS_NAMESPACE_END