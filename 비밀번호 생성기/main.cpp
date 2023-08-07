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
	// Local Time ��� ���� �ð� ���ϱ�
	const auto now{ zoned_time{ "Asia/Seoul", system_clock::now() }.get_local_time() };
	// year_month_day�� ��� ���� days���� ����
	const auto dp{ floor<days>(now) };
	// ��¥�� ��� ���� year_month_day
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
	cout << "������ ��й�ȣ��: " << pw << endl;
	return 0;
}
