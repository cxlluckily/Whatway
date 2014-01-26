#ifndef FILE_LOG_H
#define FILE_LOG_H

#include <string>

#include <boost/thread/detail/singleton.hpp>


class FileLog
{
public:
	FileLog(void);
	~FileLog(void);

	void Log(std::string log, std::string file="output");
};

typedef boost::detail::thread::singleton<FileLog> gFileLog;

#endif