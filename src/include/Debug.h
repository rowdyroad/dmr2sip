#pragma once

#include <iostream>
#include <sstream>
#include <functional>
#include <time.h>


class Debug : public std::ostream {

  private:
    typedef std::function
    <std::string (const std::string &name, const std::string &level, const std::string &data)> Formatter;

    Formatter formatter;
    bool hasFormatter = false;

    class Buffer : public std::stringbuf {

      public:
        Debug *debug;
        Buffer(std::ostream &str) : output(str) {}

        int sync () {
          static const char * labels[4] = { "error", "warn", "info", "debug" };
          if (debug->currentLevel <= debug->maxLevel) {
            if (debug->hasFormatter) {

              std::string name = debug->name;
              std::string level = labels[debug->currentLevel - 1];

              output
                << debug->formatter(name, level, str());
            }
            else {
              time_t rawtime;
              struct tm * timeinfo;
              char buffer [100];
              time(&rawtime);
              timeinfo = localtime(&rawtime);
              strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
              output
                << "[" << buffer << "] "
                << "<" << debug->name << "> "
                << (char)toupper(labels[debug->currentLevel - 1][0]) << ": "
                << str();
            }
          }
          str("");
          output.flush();
          return 0;
        }

      private:
        std::ostream &output;
    };

    Buffer buffer;

  public:
    typedef enum { quiet, error, warn, info, debug } level;

    level currentLevel = debug;
    level maxLevel = debug;
    std::string name;

    void format(const Formatter &f) {
      hasFormatter = true;
      formatter = f;
    }

    Debug(const std::string &name, std::ostream& stream = std::cout,level maxLevel = debug) :
      std::ostream(&buffer),
      maxLevel(maxLevel),
      name(name),
      buffer(stream) {
        buffer.debug = this;
      }
};

inline Debug &operator << (Debug &d, const Debug::level &level) {
  d.currentLevel = level;
  return d;
}

