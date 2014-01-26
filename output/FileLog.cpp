#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "FileLog.h"
#include "./config/configmanager.h"


FileLog::FileLog(void)
{
}


FileLog::~FileLog(void)
{
}

void FileLog::Log(std::string log, std::string file)
{
	TRACE("�������������%s\n", log.c_str());

	// ���������ģʽ��ֱ�ӷ��أ�����¼��־�ļ�
	if (gConfigManager::instance().nRunMode == 1)
		return;

	// ����Ŀ¼
	std::string sLogDir = gConfigManager::instance().m_sLogFilePath;
	boost::filesystem::path p(sLogDir);
	if (!boost::filesystem::exists(p))
	{
		boost::filesystem::create_directories(p);
	}

	std::string sLogFileName = "";
	if (file.empty())
		sLogFileName = sLogDir + "\\tradegateway";
	else
		sLogFileName = sLogDir + "\\" + file;

	sLogFileName += "_";

	boost::gregorian::date day = boost::gregorian::day_clock::local_day();
	sLogFileName += to_iso_extended_string(day);
	sLogFileName += ".log";


	std::ofstream outfile(sLogFileName.c_str(), std::ios_base::app);
	if (outfile.is_open())
	{
		boost::posix_time::ptime beginTime =  boost::posix_time::microsec_clock::local_time();
		std::string sBeginTime = boost::gregorian::to_iso_extended_string(beginTime.date()) + " " + boost::posix_time::to_simple_string(beginTime.time_of_day());
		outfile << "ʱ�䣺" << sBeginTime << "\n";

		outfile << "���ݣ�" << log << "\n";
			
		// ��һ��
		outfile << "\n";

		outfile.close();
	}

}
