#include <iostream>
#include <string>
#include <chrono>

using std::cin;
using std::cout;
using std::endl;

using std::string;
using std::to_string;

using namespace std::chrono;

string generatePassword() {
	// Local Time 기반 현재 시간 구하기
	const auto now{ zoned_time{ "Asia/Seoul", system_clock::now() }.get_local_time() };
	// year_month_day를 얻기 위해 days까지 절삭
	const auto dp{ floor<days>(now) };
	// 날짜를 얻기 위한 year_month_day
	year_month_day ymd{ dp };

	int year = static_cast<int>(ymd.year());
	int month = static_cast<unsigned>(ymd.month());

	int quotient = (2211 * month - 100 * month * (month - 1) + year) / 7;
	int remainder = (2211 * month - 100 * month * (month - 1) + year) % 7;

	string password = to_string(quotient) + to_string(remainder);

	return password;
}

auto main() -> int {
	string pw = generatePassword();
	cout << "오늘의 비밀번호는: " << pw << endl;
	return 0;
}
