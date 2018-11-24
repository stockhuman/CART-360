
/*
Ansiterm.cpp -- an Arduino library that simplifies using the ANSI terminal
control escape sequences. This allows your code to  position text exactly in the
terminal, to add colour and other effects. To use Ansiterm, an ANSI terminal must
be connected to the Arduino's serial port. The terminal built into the Arduino
IDE does not respond to the ANSI codes, but most MacOs and Linux terminals do.
In Windows, I read that Hyperterm responds to ANSI codes. Realterm responds to most
codes, but will not change the color of the foreground.

The accompanying example file illustrates the library's use.

Copyright (c) 2009 Bruce Robertson, VE9QRP. All rights reserved.

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

The latest version of this software is available at
*/

#ifndef Ansiterm_h
  #define Ansiterm_h

  #define ANSI_ESCAPE 0x1B
  #define ANSI_BRACE '['
  #define ANSI_BLACK 0
  #define ANSI_RED 1
  #define ANSI_GREEN 2
  #define ANSI_YELLOW 3
  #define ANSI_BLUE 4
  #define ANSI_MAGENTA 5
  #define ANSI_CYAN 6
  #define ANSI_WHITE 7
  #define ANSI_BOLD_ON 1
  #define ANSI_BOLD_OFF 22
  #define ANSI_UNDERLINE_ON 4
  #define ANSI_UNDERLINE_OFF 24
  #define ANSI_ITALICS_ON 3
  #define ANSI_ITALICS_OFF 23
  #define ANSI_STRIKETHROUGH_ON 9
  #define ANSI_STRIKETHROUGH_OFF 29
  #define ANSI_INVERSE_ON 7
  #define ANSI_INVERSE_OFF 27
  #define ANSI_RESET 0
  #define ANSI_DEFAULT_FOREGROUND 39
  #define ANSI_DEFAULT_BACKGROUND 49


  class Ansiterm {

    public:
      Ansiterm();
      void home();
      void xy(int x, int y);
      void up(int x);
      void down(int x);
      void forward(int x);
      void backward(int x);
      void eraseLine();
      void eraseScreen();
      void setBackgroundColor(int color);
      void setForegroundColor(int color);
      void boldOn();
      void boldOff();
      void underlineOn();
      void underlineOff();
      void italicsOn();
      void italicsOff();
      void strikethroughOn();
      void strikethroughOff();
      void inverseOn();
      void inverseOff();
      void reset();
      void defaultBackground();
      void defaultForeground();
      void fill(int x1, int y1, int x2, int y2);
    private:
      void preamble();
      void preambleAndNumberAndValue(int x, char v);
      void setAttribute(int a);
  };
#endif

Ansiterm::Ansiterm() {}

void Ansiterm::home() {
  preamble();
  Serial.write('H');
}

void Ansiterm::xy(int x, int y) {
  preamble();
  Serial.print(y, DEC);
  Serial.write(';');
  Serial.print(x, DEC);
  Serial.write('H');
}

void Ansiterm::up(int x) { preambleAndNumberAndValue(x, 'A'); }

void Ansiterm::down(int x) { preambleAndNumberAndValue(x, 'B'); }

void Ansiterm::forward(int x) { preambleAndNumberAndValue(x, 'C'); }

void Ansiterm::backward(int x) { preambleAndNumberAndValue(x, 'D'); }

void Ansiterm::eraseLine() {
  preamble();
  Serial.write('2');
  Serial.write('K');
}

void Ansiterm::eraseScreen() {
  preamble();
  Serial.write('2');
  Serial.write('J');
}

void Ansiterm::setBackgroundColor(int color) { setAttribute(color + 40); }

void Ansiterm::setForegroundColor(int color) { setAttribute(color + 30); }

void Ansiterm::boldOn() { setAttribute(ANSI_BOLD_ON); }

void Ansiterm::boldOff() { setAttribute(ANSI_BOLD_OFF); }

void Ansiterm::italicsOn() { setAttribute(ANSI_ITALICS_ON); }

void Ansiterm::italicsOff() { setAttribute(ANSI_ITALICS_OFF); }

void Ansiterm::underlineOn() { setAttribute(ANSI_UNDERLINE_ON); }

void Ansiterm::underlineOff() { setAttribute(ANSI_UNDERLINE_OFF); }

void Ansiterm::strikethroughOn() { setAttribute(ANSI_STRIKETHROUGH_ON); }

void Ansiterm::strikethroughOff() { setAttribute(ANSI_STRIKETHROUGH_OFF); }

void Ansiterm::inverseOn() { setAttribute(ANSI_INVERSE_ON); }

void Ansiterm::inverseOff() { setAttribute(ANSI_INVERSE_OFF); }

void Ansiterm::reset() { setAttribute(ANSI_RESET); }

void Ansiterm::defaultBackground() { setAttribute(ANSI_DEFAULT_BACKGROUND); }

void Ansiterm::defaultForeground() { setAttribute(ANSI_DEFAULT_FOREGROUND); }

void Ansiterm::fill(int x1, int y1, int x2, int y2) {
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      xy(x, y);
      Serial.print(' ');
    }
  }
}

/* private functions */
void Ansiterm::preamble() {
  Serial.write(ANSI_ESCAPE);
  Serial.write(ANSI_BRACE);
}

void Ansiterm::preambleAndNumberAndValue(int x, char v) {
  preamble();
  Serial.print(x, DEC);
  Serial.write(v);
}

void Ansiterm::setAttribute(int a) { preambleAndNumberAndValue(a, 'm'); }
