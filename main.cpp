#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <random>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <utility>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

// ─── 이름 데이터 ──────────────────────────────────────────────────────────────

using NamePair = std::pair<std::string, std::string>; // {표시 이름, 로마자}

static const std::vector<NamePair> KO_LAST = {
    {"김","kim"},  {"이","lee"},   {"박","park"},  {"최","choi"},  {"정","jung"},
    {"강","kang"}, {"조","cho"},   {"윤","yoon"},  {"장","jang"},  {"임","lim"},
    {"한","han"},  {"오","oh"},    {"서","seo"},   {"신","shin"},  {"권","kwon"},
    {"황","hwang"},{"안","ahn"},   {"송","song"},  {"류","ryu"},   {"전","jeon"},
    {"홍","hong"}, {"고","ko"},    {"문","moon"},  {"양","yang"},  {"손","son"},
    {"배","bae"},  {"백","baek"},  {"허","heo"},   {"유","yoo"},   {"남","nam"},
};

static const std::vector<NamePair> KO_FIRST = {
    {"민준","minjun"},   {"서준","seojun"},   {"도윤","doyun"},    {"예준","yejun"},
    {"시우","siu"},      {"주원","juwon"},    {"하준","hajun"},    {"지호","jiho"},
    {"지훈","jihun"},    {"준서","junseo"},   {"서연","seoyeon"},  {"서윤","seoyun"},
    {"지아","jia"},      {"서현","seohyun"},  {"민서","minseo"},   {"하은","haeun"},
    {"하윤","hayun"},    {"윤서","yunseo"},   {"채원","chaewon"},  {"수아","sua"},
    {"민지","minji"},    {"지원","jiwon"},    {"수빈","subin"},    {"은지","eunji"},
    {"혜원","hyewon"},   {"진호","jinho"},    {"성민","sungmin"},  {"재원","jaewon"},
    {"준혁","junhyuk"},  {"태양","taeyang"},  {"재민","jaemin"},   {"현우","hyunwoo"},
    {"동현","donghyun"}, {"지민","jimin"},    {"건우","gunwoo"},   {"우진","woojin"},
    {"나연","nayeon"},   {"지현","jihyun"},   {"예린","yerin"},    {"보미","bomi"},
    {"소율","soyul"},    {"아린","arin"},     {"지유","jiyu"},     {"다은","daeun"},
    {"예원","yewon"},    {"소현","sohyun"},   {"민수","minsu"},    {"진영","jinyoung"},
};

static const std::vector<std::string> EN_FIRST = {
    "James","John","Robert","Michael","William","David","Richard","Joseph",
    "Thomas","Charles","Christopher","Daniel","Matthew","Anthony","Donald",
    "Mary","Patricia","Jennifer","Linda","Barbara","Susan","Jessica","Sarah",
    "Karen","Lisa","Nancy","Betty","Margaret","Sandra","Ashley","Dorothy",
    "Emma","Olivia","Sophia","Ava","Isabella","Mia","Charlotte","Amelia",
    "Liam","Noah","Oliver","Elijah","Lucas","Mason","Logan","Ethan",
};

static const std::vector<std::string> EN_LAST = {
    "Smith","Johnson","Williams","Brown","Jones","Garcia","Miller","Davis",
    "Wilson","Taylor","Anderson","Thomas","Jackson","White","Harris","Martin",
    "Lee","Thompson","Clark","Lewis","Robinson","Walker","Hall","Young",
    "Allen","King","Wright","Scott","Green","Baker","Adams","Nelson",
    "Carter","Mitchell","Perez","Roberts","Turner","Phillips","Campbell",
};

static const std::vector<std::string> DOMAINS = {
    "gmail.com","naver.com","kakao.com","daum.net",
    "outlook.com","yahoo.com","hotmail.com","example.com",
    "test.org","mail.net","sample.io","dev.com",
};

// ─── 전역 RNG ─────────────────────────────────────────────────────────────────

static std::mt19937 rng;

template<typename T>
const T& pick(const std::vector<T>& v) {
    std::uniform_int_distribution<std::size_t> dist(0, v.size() - 1);
    return v[dist(rng)];
}

// ─── User ─────────────────────────────────────────────────────────────────────

struct User {
    int id;
    std::string name;
    std::string email;
};

// ─── 이름 생성 ────────────────────────────────────────────────────────────────

static NamePair generateNameKo() {
    const auto& [ln, lr] = pick(KO_LAST);
    const auto& [fn, fr] = pick(KO_FIRST);
    return {ln + fn, lr + fr};
}

static NamePair generateNameEn() {
    const std::string& first = pick(EN_FIRST);
    const std::string& last  = pick(EN_LAST);
    std::string fl = first, ll = last;
    std::transform(fl.begin(), fl.end(), fl.begin(), ::tolower);
    std::transform(ll.begin(), ll.end(), ll.begin(), ::tolower);
    return {first + " " + last, fl + "." + ll};
}

static NamePair generateName(const std::string& lang) {
    if (lang == "ko") return generateNameKo();
    if (lang == "en") return generateNameEn();
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng) < 0.6 ? generateNameKo() : generateNameEn();
}

// ─── 이메일 생성 (중복 방지) ──────────────────────────────────────────────────

static std::string generateEmail(const std::string& roman, std::set<std::string>& used) {
    const std::string& domain = pick(DOMAINS);

    // 알파벳·숫자·점 외 문자 제거
    std::string base;
    for (char c : roman)
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '.')
            base += c;
    if (base.empty()) base = "user";

    std::string email = base + "@" + domain;
    if (!used.count(email)) { used.insert(email); return email; }

    for (int i = 2; i < 10000; ++i) {
        email = base + std::to_string(i) + "@" + domain;
        if (!used.count(email)) { used.insert(email); return email; }
    }
    // 최후 수단: 나노초 타임스탬프
    auto ts = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    email = base + std::to_string(ts) + "@" + domain;
    used.insert(email);
    return email;
}

// ─── 더미 사용자 생성 ─────────────────────────────────────────────────────────

static std::vector<User> generateUsers(int count, int startId,
                                       const std::string& lang,
                                       std::set<std::string>& usedEmails) {
    std::vector<User> users;
    users.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        auto [name, roman] = generateName(lang);
        std::string email  = generateEmail(roman, usedEmails);
        users.push_back({startId + i, std::move(name), std::move(email)});
    }
    return users;
}

// ─── JSON DB I/O ──────────────────────────────────────────────────────────────

static std::pair<std::vector<User>, int> loadDb(const std::string& path) {
    std::vector<User> users;
    int nextId = 1;
    std::ifstream ifs(path);
    if (!ifs) return {users, nextId};
    try {
        json j;
        ifs >> j;
        nextId = j.value("nextId", 1);
        for (const auto& item : j.at("users")) {
            users.push_back({
                item.at("id").get<int>(),
                item.at("name").get<std::string>(),
                item.at("email").get<std::string>()
            });
        }
    } catch (const std::exception& e) {
        std::cerr << "[WARN] JSON 파싱 실패: " << e.what() << " — 새로 시작합니다.\n";
        return {{}, 1};
    }
    return {users, nextId};
}

static void saveDb(const std::string& path,
                   const std::vector<User>& users, int nextId) {
    fs::path p(path);
    if (p.has_parent_path())
        fs::create_directories(p.parent_path());

    json j;
    j["nextId"] = nextId;
    j["users"]  = json::array();
    for (const auto& u : users)
        j["users"].push_back({{"id", u.id}, {"name", u.name}, {"email", u.email}});

    std::ofstream ofs(path);
    if (!ofs) throw std::runtime_error("파일 열기 실패: " + path);
    ofs << j.dump(2);
}

// ─── 출력 헬퍼 ────────────────────────────────────────────────────────────────

static void printUsers(const std::vector<User>& users, const std::string& label) {
    std::cout << "\n[" << label << "]\n";
    std::cout << std::string(58, '-') << "\n";
    for (const auto& u : users) {
        std::cout << "  [" << std::setw(4) << std::right << u.id << "]  "
                  << std::left << u.name << "  /  " << u.email << "\n";
    }
    std::cout << std::string(58, '-') << "\n";
}

// ─── CLI ──────────────────────────────────────────────────────────────────────

struct Config {
    int         count   = 10;
    std::string output;          // 비어 있으면 argv[0] 기준으로 자동 계산
    bool        append  = false;
    std::string lang    = "mixed";
    unsigned    seed    = 0;
    bool        hasSeed = false;
    bool        preview = false;
};

static void printHelp(const char* prog) {
    std::cout <<
        "사용법: " << prog << " [옵션]\n\n"
        "옵션:\n"
        "  -n <수>               생성할 사용자 수 (기본: 10)\n"
        "  -o <경로>             출력 파일 경로\n"
        "                        (기본: ../DataPersistence/build/users.json)\n"
        "  --append              기존 데이터에 추가 (미지정 시 덮어쓰기)\n"
        "  --lang <ko|en|mixed>  이름 언어 (기본: mixed)\n"
        "  --seed <값>           랜덤 시드 (재현 가능한 데이터 생성)\n"
        "  --preview             저장하지 않고 생성 결과만 출력\n"
        "  -h, --help            도움말\n\n"
        "예시:\n"
        "  DummyDataGenerator                     10개 생성 (기존 교체)\n"
        "  DummyDataGenerator -n 50               50개 생성\n"
        "  DummyDataGenerator -n 20 --append      기존 데이터에 20개 추가\n"
        "  DummyDataGenerator -n 30 --lang en     영어 이름 30개\n"
        "  DummyDataGenerator -n 100 --seed 42    시드 고정으로 재현\n"
        "  DummyDataGenerator --preview           저장 없이 미리보기\n";
}

static Config parseArgs(int argc, char* argv[]) {
    Config cfg;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-h" || a == "--help") {
            printHelp(argv[0]); std::exit(0);
        } else if (a == "-n") {
            if (++i >= argc) throw std::runtime_error("-n 뒤에 값이 필요합니다.");
            cfg.count = std::stoi(argv[i]);
            if (cfg.count <= 0) throw std::runtime_error("-n 은 1 이상이어야 합니다.");
        } else if (a == "-o") {
            if (++i >= argc) throw std::runtime_error("-o 뒤에 경로가 필요합니다.");
            cfg.output = argv[i];
        } else if (a == "--append") {
            cfg.append = true;
        } else if (a == "--lang") {
            if (++i >= argc) throw std::runtime_error("--lang 뒤에 값이 필요합니다.");
            cfg.lang = argv[i];
            if (cfg.lang != "ko" && cfg.lang != "en" && cfg.lang != "mixed")
                throw std::runtime_error("--lang 은 ko / en / mixed 중 하나여야 합니다.");
        } else if (a == "--seed") {
            if (++i >= argc) throw std::runtime_error("--seed 뒤에 값이 필요합니다.");
            cfg.seed    = static_cast<unsigned>(std::stoul(argv[i]));
            cfg.hasSeed = true;
        } else if (a == "--preview") {
            cfg.preview = true;
        } else {
            throw std::runtime_error("알 수 없는 옵션: " + a);
        }
    }
    return cfg;
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8 출력
#endif

    Config cfg;
    try {
        cfg = parseArgs(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n\n";
        printHelp(argv[0]);
        return 1;
    }

    // 기본 출력 경로: exe 기준 ../../DataPersistence/build/users.json
    if (cfg.output.empty()) {
        fs::path exeDir = fs::path(argv[0]).parent_path();
        fs::path target = exeDir / ".." / ".." / "DataPersistence" / "build" / "users.json";
        cfg.output = fs::weakly_canonical(target).string();
    }

    // RNG 초기화
    if (cfg.hasSeed) {
        rng.seed(cfg.seed);
        std::cout << "[INFO] 랜덤 시드 : " << cfg.seed << "\n";
    } else {
        rng.seed(std::random_device{}());
    }

    std::cout << "[INFO] 대상 파일  : " << cfg.output << "\n";

    // 기존 DB 로드 (--append 모드)
    std::vector<User> existing;
    int nextId = 1;
    if (cfg.append && !cfg.preview) {
        auto [u, id] = loadDb(cfg.output);
        existing = std::move(u);
        nextId   = id;
        std::cout << "[INFO] 기존 사용자: " << existing.size()
                  << "명  (nextId=" << nextId << ")\n";
    }

    // 기존 이메일 수집 (중복 방지)
    std::set<std::string> usedEmails;
    for (const auto& u : existing) usedEmails.insert(u.email);

    // 더미 데이터 생성
    std::cout << "[INFO] " << cfg.count << "명 생성 중... (lang=" << cfg.lang << ")\n";
    auto newUsers    = generateUsers(cfg.count, nextId, cfg.lang, usedEmails);
    int finalNextId  = nextId + cfg.count;

    if (cfg.preview) {
        // ── 미리보기 모드 ──────────────────────────────────────────────────
        json j;
        j["nextId"] = finalNextId;
        j["users"]  = json::array();
        for (const auto& u : newUsers)
            j["users"].push_back({{"id", u.id}, {"name", u.name}, {"email", u.email}});
        std::cout << "\n[PREVIEW] 생성될 데이터 (저장 안 함):\n"
                  << j.dump(2) << "\n";
    } else {
        // ── 저장 모드 ──────────────────────────────────────────────────────
        std::vector<User> allUsers = std::move(existing);
        allUsers.insert(allUsers.end(), newUsers.begin(), newUsers.end());

        try {
            saveDb(cfg.output, allUsers, finalNextId);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] 저장 실패: " << e.what() << "\n";
            return 1;
        }

        std::cout << "[OK] 저장 완료\n"
                  << "[OK] 전체 사용자: " << allUsers.size()
                  << "명  (신규: " << cfg.count
                  << "명,  nextId: " << finalNextId << ")\n";
        printUsers(newUsers, "신규 생성된 사용자");
    }

    return 0;
}
