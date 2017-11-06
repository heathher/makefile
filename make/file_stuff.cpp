#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <climits>
#include <time.h>

bool operator > (const struct tm &left, const struct tm &right)  
{
	if (left.tm_year > right.tm_year)
		return true;
	if (left.tm_year < right.tm_year)
		return false;
	if (left.tm_mon > right.tm_mon)
		return true;
	if (left.tm_mon < right.tm_mon)
		return false;
	if (left.tm_mday > right.tm_mday)
		return true;
	if (left.tm_mday < right.tm_mday)
		return false;
	if (left.tm_hour > right.tm_hour)
		return true;
	if (left.tm_hour < right.tm_hour)
		return false;
	if (left.tm_min > right.tm_min)
		return true;
	return false;
}

bool fileExists(const std::string &filename)  //доступ к файлу
{
	return (access(filename.c_str(), F_OK) != -1);
}

bool buildIsNecessary(const std::string &target, const std::string &dependency)    //необходимо ли построение (цель, зависимость)
{
	if ((!fileExists(target)) || (!fileExists(dependency)))
		return true;
	struct tm *time_ptr;
	time_t t_target;
	time_t t_dep;
	struct stat attrib;
	stat(target.c_str(), &attrib);
    time_ptr = gmtime(&(attrib.st_mtime));
    t_target = mktime(time_ptr);
    stat(dependency.c_str(), &attrib);   //
    time_ptr = gmtime(&(attrib.st_mtime));
    t_dep = mktime(time_ptr);
	return difftime(t_dep, t_target) > 0;
}

std::string getWorkingDirectory()
{
	const char *cwd= getenv("PWD");
	if (cwd == NULL)
		return std::string("");
	return std::string(cwd);
}