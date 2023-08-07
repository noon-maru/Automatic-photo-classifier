#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <chrono>

using std::cin;
using std::cout;
using std::endl;

using std::string;
using std::to_string;

using std::map;

using std::set;

using std::vector;

using namespace std::chrono;
namespace fs = std::filesystem;

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

void DivideFilesByExtension() {
	map<string, vector<fs::path>> filesByExtension{ };

	// 현재 디렉터리 내의 모든 파일에 대해 파일 확장자별로 분류
	for (const auto& entry : fs::directory_iterator{ "." }) {
		if (entry.is_regular_file()) {
			string extension = entry.path().extension().string();

			// 대문자 확장자를 소문자로 변환, C++20 필요
			std::ranges::transform(extension, extension.begin(), [](char c) { return tolower(c); });

			filesByExtension[extension].push_back(entry.path());
		}
	}



	// 확장자별로 디렉터리 생성 후 파일 이동
	for (const auto& [extension, files] : filesByExtension) {
		string rawDirectoryName = "raw";
		string jpgDirectoryName = "jpg";

		if (extension == ".crw" or extension == ".cr2" or extension == ".cr3" or
			extension == ".nef" or extension == ".nrw" or
			extension == ".pef" or extension == ".dng" or
			extension == ".raf" or extension == ".srw" or
			extension == ".orf" or extension == ".srf" or extension == ".sr2" or extension == ".arw" or
			extension == ".rw2" or extension == ".3fr" or
			extension == ".dcr" or extension == ".kdc" or
			extension == ".mrw" or
			extension == ".rwl" or extension == ".dng" or
			extension == ".mos" or
			extension == ".x3f" or
			extension == ".gpr") {
			fs::create_directory(rawDirectoryName);

			for (const auto& file : files) {
				fs::path destination = rawDirectoryName + "/" + file.filename().string();
				fs::rename(file, destination);
			}
		}
		else if (extension == ".jpg") {
			fs::create_directory(jpgDirectoryName);
			for (const auto& file : files) {
				fs::path destination = jpgDirectoryName + "/" + file.filename().string();
				fs::rename(file, destination);
			}
		}
	}
}

void SyncDirectories() {
	fs::path jpgDirectory = "jpg";
	fs::path rawDirectory = "raw";

	set<string> jpgFileNames{ };
	set<string> rawFileNames{ };

	// jpg 디렉터리의 파일 이름 집합 구성
	for (const auto& entry : fs::directory_iterator{ jpgDirectory }) {
		if (entry.is_regular_file()) {
			fs::path jpgFilePath = entry.path().filename();
			jpgFileNames.insert(jpgFilePath.stem().string());
		}
	}

	// raw 디렉터리의 파일 이름 집합 구성
	for (const auto& entry : fs::directory_iterator{ rawDirectory }) {
		if (entry.is_regular_file()) {
			fs::path rawFilePath = entry.path().filename();
			rawFileNames.insert(rawFilePath.stem().string());
		}
	}

	// jpg 디렉터리와 raw 디렉터리 간의 동기화 수행
	for (const auto& entry : fs::directory_iterator{ jpgDirectory }) {
		if (entry.is_regular_file()) {
			fs::path jpgFilePath = entry.path().filename();
			string fileName = jpgFilePath.stem().string();

			if (not rawFileNames.count(fileName))
				fs::remove(entry.path());
		}
	}

	for (const auto& entry : fs::directory_iterator{ rawDirectory }) {
		if (entry.is_regular_file()) {
			fs::path rawFilePath = entry.path().filename();
			string fileName = rawFilePath.stem().string();

			if (not jpgFileNames.count(fileName))
				fs::remove(entry.path());
		}
	}
}

void OrganizeFiles(const string& format, const string& placeName, int startNumber, int middleNumber) {
	int currentNumber = 1;
	string folderName = "";

	// 현재 디렉터리 내의 모든 파일에 대해 처리
	vector<fs::path> files{ };
	for (const auto& entry : fs::directory_iterator{ format })
		if (entry.is_regular_file())
			files.push_back(entry.path());

	// 파일 이름 정렬
	sort(files.begin(), files.end());

	// 파일 이름 변경
	for (const auto& file : files) {
		string extension = file.extension().string();
		string fileName = file.filename().string();

		// 파일 이름의 숫자 부분 추출
		auto extensionPos = fileName.find_last_of('.');
		auto numberPos = fileName.find_first_of("0123456789");

		string numberString{ };
		if (numberPos != string::npos)
			numberString = fileName.substr(numberPos, extensionPos);
		else
			continue; // 숫자가 없는 파일은 건너뜀

		int fileNumber = stoi(numberString);
		string newFileName{ };


		if (startNumber <= fileNumber and fileNumber < middleNumber or
			middleNumber < startNumber) {
			newFileName = placeName + "-" + to_string(currentNumber) + extension;
			++currentNumber;
		}
		else if (fileNumber >= middleNumber)
			break;

		fs::rename(file, file.parent_path() / newFileName);
	}
}


auto main() -> int {
	// 비밀번호 생성
	string pw = generatePassword();

	// 비밀번호 생성
	while (true) {
		cout << "비밀번호를 입력하세요." << endl;
		string inputPW{ }; cin >> inputPW;
		if (inputPW == pw) {
			cout << "로그인 되었습니다." << endl;
			system("pause");
			break;
		}
	}
	system("cls"); // 화면 초기화

	cout << "1: 확장자 별 분류, 2: jpg, raw 디렉터리 동기화, 3: 파일 정리" << endl;
	cout << "수행할 기능의 번호를 입력하세요: ";
	int action{ }; cin >> action;

	switch (action) {
	case 1:
		DivideFilesByExtension();
		cout << "파일 확장자별로 분류되었습니다." << endl;
		break;

	case 2:
		SyncDirectories();
		cout << "jpg, raw 디렉터리가 동기화되었습니다." << endl;
		break;

	case 3: {
		while (true) {
			cout << "장소 이름을 입력하세요: ";
			string placeName{ }; cin >> placeName;
			cout << "시작 번호를 입력하세요: ";
			int startNumber{ }; cin >> startNumber;
			cout << "중간 번호를 입력하세요: ";
			int middleNumber{ }; cin >> middleNumber;

			OrganizeFiles("jpg", placeName, startNumber, middleNumber);
			OrganizeFiles("raw", placeName, startNumber, middleNumber);
			cout << "파일이 정리되었습니다." << endl;
		}
		break;
	}

	default:
		cout << "유효하지 않은 번호입니다." << endl;
		break;
	}

	return 0;
}
