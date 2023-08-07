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

void DivideFilesByExtension() {
	map<string, vector<fs::path>> filesByExtension{ };

	// ���� ���͸� ���� ��� ���Ͽ� ���� ���� Ȯ���ں��� �з�
	for (const auto& entry : fs::directory_iterator{ "." }) {
		if (entry.is_regular_file()) {
			string extension = entry.path().extension().string();

			// �빮�� Ȯ���ڸ� �ҹ��ڷ� ��ȯ, C++20 �ʿ�
			std::ranges::transform(extension, extension.begin(), [](char c) { return tolower(c); });

			filesByExtension[extension].push_back(entry.path());
		}
	}



	// Ȯ���ں��� ���͸� ���� �� ���� �̵�
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

	// jpg ���͸��� ���� �̸� ���� ����
	for (const auto& entry : fs::directory_iterator{ jpgDirectory }) {
		if (entry.is_regular_file()) {
			fs::path jpgFilePath = entry.path().filename();
			jpgFileNames.insert(jpgFilePath.stem().string());
		}
	}

	// raw ���͸��� ���� �̸� ���� ����
	for (const auto& entry : fs::directory_iterator{ rawDirectory }) {
		if (entry.is_regular_file()) {
			fs::path rawFilePath = entry.path().filename();
			rawFileNames.insert(rawFilePath.stem().string());
		}
	}

	// jpg ���͸��� raw ���͸� ���� ����ȭ ����
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

	// ���� ���͸� ���� ��� ���Ͽ� ���� ó��
	vector<fs::path> files{ };
	for (const auto& entry : fs::directory_iterator{ format })
		if (entry.is_regular_file())
			files.push_back(entry.path());

	// ���� �̸� ����
	sort(files.begin(), files.end());

	// ���� �̸� ����
	for (const auto& file : files) {
		string extension = file.extension().string();
		string fileName = file.filename().string();

		// ���� �̸��� ���� �κ� ����
		auto extensionPos = fileName.find_last_of('.');
		auto numberPos = fileName.find_first_of("0123456789");

		string numberString{ };
		if (numberPos != string::npos)
			numberString = fileName.substr(numberPos, extensionPos);
		else
			continue; // ���ڰ� ���� ������ �ǳʶ�

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
	// ��й�ȣ ����
	string pw = generatePassword();

	// ��й�ȣ ����
	while (true) {
		cout << "��й�ȣ�� �Է��ϼ���." << endl;
		string inputPW{ }; cin >> inputPW;
		if (inputPW == pw) {
			cout << "�α��� �Ǿ����ϴ�." << endl;
			system("pause");
			break;
		}
	}
	system("cls"); // ȭ�� �ʱ�ȭ

	cout << "1: Ȯ���� �� �з�, 2: jpg, raw ���͸� ����ȭ, 3: ���� ����" << endl;
	cout << "������ ����� ��ȣ�� �Է��ϼ���: ";
	int action{ }; cin >> action;

	switch (action) {
	case 1:
		DivideFilesByExtension();
		cout << "���� Ȯ���ں��� �з��Ǿ����ϴ�." << endl;
		break;

	case 2:
		SyncDirectories();
		cout << "jpg, raw ���͸��� ����ȭ�Ǿ����ϴ�." << endl;
		break;

	case 3: {
		while (true) {
			cout << "��� �̸��� �Է��ϼ���: ";
			string placeName{ }; cin >> placeName;
			cout << "���� ��ȣ�� �Է��ϼ���: ";
			int startNumber{ }; cin >> startNumber;
			cout << "�߰� ��ȣ�� �Է��ϼ���: ";
			int middleNumber{ }; cin >> middleNumber;

			OrganizeFiles("jpg", placeName, startNumber, middleNumber);
			OrganizeFiles("raw", placeName, startNumber, middleNumber);
			cout << "������ �����Ǿ����ϴ�." << endl;
		}
		break;
	}

	default:
		cout << "��ȿ���� ���� ��ȣ�Դϴ�." << endl;
		break;
	}

	return 0;
}
