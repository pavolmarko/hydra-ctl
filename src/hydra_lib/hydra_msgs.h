#ifndef __HYDRA_CTL_HYDRA_MSGS_H__
#define __HYDRA_CTL_HYDRA_MSGS_H__

#include <stdint.h>


namespace hydra {

const uint8_t REQ_HELLO = 0x50;
const uint8_t RSP_HELLO = 0x01;

const uint8_t REQ_OPEN_START = 0xBE;
const uint8_t REQ_OPEN_CONTINUE = 0xC0;

const uint8_t REQ_CLOSE_START = 0xAA;
const uint8_t REQ_CLOSE_CONTINUE = 0xC2;

const uint8_t REQ_GET_STATE = 0xA0;

const uint8_t RSP_POS_FULLY_OPEN = 0xA2;
const uint8_t RSP_POS_FULLY_CLOSED = 0xA1;

const uint8_t RSP_OPENING = 0xC0;
const uint8_t RSP_CLOSING = 0xC2;
// Probably?
const uint8_t RSP_POS_INBETWEEN = 0xA0;
const uint8_t REQ_GET_POS = 0xA0;

enum class Error {
  ERROR_UNKNOWN = 0,
  CONF_RECV_TIMEOUT = 1,
  RECV_TIMEOUT = 2,
  RECV_BAD_COUNTER = 3,
  SERVER_HELLO_TIMEOUT = 4,
  SERVER_HELLO_INVALID = 5,
};

// Maybe the response is always a status

// 0xMP  M = motor, P = position
//  M: A = no movement
//  M: C = moevement
//  P: 0: in between
//  P: 2: open
//  P: 1: closed
} // hydra

#endif  // __HYDRA_CTL_HYDRA_MSGS_H__

