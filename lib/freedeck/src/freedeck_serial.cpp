#include "freedeck_serial.hpp"
#include "GFX.hpp"
#include "freedeck.hpp"
#include "init.hpp"
#include "settings.hpp"
#include "util.hpp"
#include "version.hpp"
#include <bsp/board.h>
#include <f_util.h>
#include <fd_usb.hpp>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <tusb.h>
void write_serial_line(const char *line) {
  const uint32_t len = strlen(line) + 3;
  char lineCRN[len];
  strcpy(lineCRN, line);
  lineCRN[len - 3] = '\r';
  lineCRN[len - 2] = '\n';
  lineCRN[len - 1] = '\0';
  tud_cdc_write_str(lineCRN);
  tud_cdc_write_flush();
}

void write_serial_char(const char chr) {
  tud_cdc_write_char(chr);
  tud_cdc_write_flush();
}

void write_serial(const char *line) {
  tud_cdc_write_str(line);
  tud_cdc_write_flush();
}

uint32_t combine_bytes(char *numbers, uint8_t len) {
  unsigned long int number = 0;
  for (char i = 0; i < len; i++) {
    if (numbers[i] == 13)
      break;
    number |= numbers[i] << (i * 8);
  }
  return number;
}

uint32_t read_serial_binary() {
  uint32_t available = tud_cdc_available();
  while (!available) {
    available = tud_cdc_available();
  }
  char buf[available];
  for (int i = 0; i < available; i++) {
    buf[i] = tud_cdc_read_char();
    if (buf[i] == '\n') {
      buf[i] = '\0';
      return combine_bytes(buf, i);
    }
  }
  return INT32_MAX;
}

char *read_serial_string(char *serial_string, uint32_t max_len) {
  uint32_t available = tud_cdc_available();
  while (!available) {
    available = tud_cdc_available();
  }
  char buf[available];
  if (available == 0)
    return serial_string;
  for (int i = 0; i < available; i++) {

    buf[i] = tud_cdc_read_char();
    if (buf[i] == '\n') {
      buf[i] = '\0';
      strncpy(serial_string, buf, i);
      return serial_string;
    }
  }
  return serial_string;
}

uint32_t _get_file_size() {
  char numberChars[10];
  read_serial_string(numberChars, 10);
  return atol(numberChars);
}

void _save_new_config_from_serial() {
  // open temp file
  f_close(&fil);
  FIL tmp_fil;
  f_open(&tmp_fil, CONFIG_NAME_TEMP, FA_CREATE_ALWAYS | FA_WRITE);
  //
  uint32_t fileSize = _get_file_size();
  set_mux_address(0);
  oled[0]->drawString(0, 2, "Updating!", 2);
  oled[0]->display();
  uint32_t receivedBytes = 0;
  uint32_t chunkLength;
  int32_t ellapsed = board_millis();
  do {
    while (!tud_cdc_available()) {
    }
    char input[1024];
    chunkLength = tud_cdc_read(input, 1024);
    receivedBytes = receivedBytes + chunkLength;

    if (board_millis() - ellapsed > 250 || receivedBytes == fileSize) {
      set_mux_address(1);
      oled[1]->drawProgressBar(0, 0, 128, 64, (float)receivedBytes / (float)fileSize * 100);
      oled[1]->display();
      ellapsed = board_millis();
      char num_char[10];
      sprintf(num_char, "%d", receivedBytes);
      write_serial_line(num_char);
    }
    f_write(&tmp_fil, input, chunkLength, NULL);
  } while (receivedBytes < fileSize);
  f_close(&tmp_fil);

  if (receivedBytes == fileSize) {
    f_unlink(CONFIG_NAME_OLD);               // delete old old config
    f_rename(CONFIG_NAME, CONFIG_NAME_OLD);  // rename current to old
    f_rename(CONFIG_NAME_TEMP, CONFIG_NAME); // make tmp to current
  }
  f_open(&fil, CONFIG_NAME, FA_READ);
}

void _dump_config_over_serial() {
  f_lseek(&fil, 0);

  uint32_t file_size = f_size(&fil);
  char f_size_str[10];
  sprintf(f_size_str, "%d", file_size);
  write_serial_line(f_size_str);
  UINT read_len = 0;
  char buff[CFG_TUD_CDC_TX_BUFSIZE];
  do {
    f_read(&fil, buff, sizeof(buff), &read_len);
    while (!tud_cdc_write_available()) {
    }
    tud_cdc_write(buff, read_len);
    tud_cdc_write_flush();
  } while (read_len == sizeof(buff));
  tud_cdc_write_flush();
}

void sdcard() { load_page(0); }

void oled_clear() {
  char display = read_serial_binary();
  if (display < BD_COUNT) {
    set_mux_address(display);
    oled[display]->clear();
    oled[display]->display();
  } else {
    for (int i = 0; i < BD_COUNT; i++) {
      set_mux_address(i);
      oled[i]->clear();
      oled[i]->display();
    }
  }
}

void oled_power() {
  char display = read_serial_binary();
  char power_state = read_serial_binary();
  if (power_state > 1)
    return;
  if (display < BD_COUNT) {
    set_mux_address(display);
    oled[display]->displayON(power_state);
  } else {
    for (int i = 0; i < BD_COUNT; i++) {
      set_mux_address(i);
      oled[i]->displayON(power_state);
    }
  }
}

void oled_write_line() {
  uint8_t display = read_serial_binary();
  uint8_t row = read_serial_binary();
  uint8_t font_size = read_serial_binary();
  font_size > 1 ? font_size : 1;
  uint8_t max_len = 10;
  char input[max_len + 1];
  memset(input, '\0', sizeof(input));

  read_serial_string(input, max_len);
  if (input) {
    set_mux_address(display);
    oled[display]->drawString(0, 8 * row, input, font_size);
    oled[display]->display();
  }
}

void oled_write_data() {
  uint8_t display = read_serial_binary();

  uint16_t received = 0;
  unsigned char buffer[1024];
  do {
    while (!tud_cdc_available()) {
    };
    char temp[64];
    size_t len = tud_cdc_read(temp, 64);
    memcpy(&buffer[received], temp, len);
    received += len;
  } while (received < 1024);

  set_mux_address(display);
  oled[display]->display(buffer);
}

void serial_api(uint32_t command) {
#ifdef WAKE_ON_SERIAL
  wake_display_if_needed();
#endif
  if (command == 0x10) { // get firmware version
    write_serial_line(VERSION);
  }
  if (command == 0x11) { // board type
    write_serial_line("rp2040,pi,pico");
  }
  if (command == 0x20) { // read config
    _dump_config_over_serial();
  }
  if (command == 0x21) { // write config
    _save_new_config_from_serial();
    post_setup();
  }
  if (command == 0x30) { // get current page
    char cur_pag_str[6];
    sprintf(cur_pag_str, "%d", current_page);
    write_serial_line(cur_pag_str);
  }
  if (command == 0x31) { // set current page
    char tar_pag_str[6];
    read_serial_string(tar_pag_str, 6);
    uint16_t target_page = strtol(tar_pag_str, NULL, 10);
    if (target_page <= page_count) {
      load_page(target_page);
    } else {
      write_serial_line(ERROR);
    }
  }
  if (command == 0x32) { // get page count
    char pag_count_str[6];
    sprintf(pag_count_str, "%d", page_count);
    write_serial_line(pag_count_str);
  }
  if (command == 0x40) { // clear screen
    oled_clear();
  }
  if (command == 0x41) { // turn screen on/off
    oled_power();
  }
  if (command == 0x42) { // oled write text
    oled_write_line();
  }
  if (command == 0x43) { // oled send data/image
    oled_write_data();
  }
}

void cdc_task(void) {

  if (!tud_cdc_available())
    return;
  uint32_t start = read_serial_binary();
  if (start != 0x3)
    return;
  uint32_t command = read_serial_binary();
  serial_api(command);
}