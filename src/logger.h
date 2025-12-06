#ifndef LOGGER_H
#define LOGGER_H

#ifdef DEBUG
#define DBG(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)       /* Debug with no line feed */
#define DBGL(fmt, ...) Serial.printf(fmt "\n", ##__VA_ARGS__) /* Debug Line */
#else
#define DBG(fmt, ...)
#define DBGL(fmt, ...)
#endif

#endif // "LOGGER_H"