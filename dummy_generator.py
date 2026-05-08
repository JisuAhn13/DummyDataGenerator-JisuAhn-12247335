#!/usr/bin/env python3
"""
DummyDataGenerator - DataPersistence.exe 용 테스트 데이터 생성 도구
users.json 형식의 더미 사용자 데이터를 생성하여 DB(파일)에 추가합니다.
"""

import json
import argparse
import random
import os
import sys
from datetime import datetime

# ─── 이름 데이터 ───────────────────────────────────────────────────────────────

KO_LAST_NAMES = [
    "김", "이", "박", "최", "정", "강", "조", "윤", "장", "임",
    "한", "오", "서", "신", "권", "황", "안", "송", "류", "전",
    "홍", "고", "문", "양", "손", "배", "백", "허", "유", "남",
]

KO_LAST_ROMAN = {
    "김": "kim",  "이": "lee",   "박": "park",  "최": "choi", "정": "jung",
    "강": "kang", "조": "cho",   "윤": "yoon",  "장": "jang", "임": "lim",
    "한": "han",  "오": "oh",    "서": "seo",   "신": "shin", "권": "kwon",
    "황": "hwang","안": "ahn",   "송": "song",  "류": "ryu",  "전": "jeon",
    "홍": "hong", "고": "ko",    "문": "moon",  "양": "yang", "손": "son",
    "배": "bae",  "백": "baek",  "허": "heo",   "유": "yoo",  "남": "nam",
}

KO_FIRST_NAMES = [
    "민준", "서준", "도윤", "예준", "시우", "주원", "하준", "지호", "지훈", "준서",
    "서연", "서윤", "지아", "서현", "민서", "하은", "하윤", "윤서", "채원", "수아",
    "민지", "지원", "수빈", "은지", "혜원", "진호", "성민", "재원", "준혁", "태양",
    "재민", "현우", "동현", "지민", "건우", "우진", "민혁", "승우", "진우", "태민",
    "나연", "지현", "예린", "보미", "소율", "아린", "지유", "다은", "예원", "소현",
]

KO_FIRST_ROMAN = {
    "민준": "minjun",   "서준": "seojun",   "도윤": "doyun",   "예준": "yejun",
    "시우": "siu",      "주원": "juwon",    "하준": "hajun",   "지호": "jiho",
    "지훈": "jihun",    "준서": "junseo",   "서연": "seoyeon", "서윤": "seoyun",
    "지아": "jia",      "서현": "seohyun",  "민서": "minseo",  "하은": "haeun",
    "하윤": "hayun",    "윤서": "yunseo",   "채원": "chaewon", "수아": "sua",
    "민지": "minji",    "지원": "jiwon",    "수빈": "subin",   "은지": "eunji",
    "혜원": "hyewon",   "진호": "jinho",    "성민": "sungmin", "재원": "jaewon",
    "준혁": "junhyuk",  "태양": "taeyang",  "재민": "jaemin",  "현우": "hyunwoo",
    "동현": "donghyun", "지민": "jimin",    "건우": "gunwoo",  "우진": "woojin",
    "민혁": "minhyuk",  "승우": "seungwoo", "진우": "jinwoo",  "태민": "taemin",
    "나연": "nayeon",   "지현": "jihyun",   "예린": "yerin",   "보미": "bomi",
    "소율": "soyul",    "아린": "arin",     "지유": "jiyu",    "다은": "daeun",
    "예원": "yewon",    "소현": "sohyun",
}

EN_FIRST_NAMES = [
    "James", "John", "Robert", "Michael", "William", "David", "Richard", "Joseph",
    "Thomas", "Charles", "Christopher", "Daniel", "Matthew", "Anthony", "Donald",
    "Mary", "Patricia", "Jennifer", "Linda", "Barbara", "Susan", "Jessica", "Sarah",
    "Karen", "Lisa", "Nancy", "Betty", "Margaret", "Sandra", "Ashley", "Dorothy",
    "Emma", "Olivia", "Sophia", "Ava", "Isabella", "Mia", "Charlotte", "Amelia",
    "Harper", "Evelyn", "Liam", "Noah", "Oliver", "Elijah", "Lucas", "Mason",
]

EN_LAST_NAMES = [
    "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
    "Wilson", "Taylor", "Anderson", "Thomas", "Jackson", "White", "Harris", "Martin",
    "Lee", "Thompson", "Clark", "Lewis", "Robinson", "Walker", "Hall", "Young",
    "Allen", "King", "Wright", "Scott", "Green", "Baker", "Adams", "Nelson",
    "Carter", "Mitchell", "Perez", "Roberts", "Turner", "Phillips", "Campbell",
]

EMAIL_DOMAINS = [
    "gmail.com", "naver.com", "kakao.com", "daum.net",
    "outlook.com", "yahoo.com", "hotmail.com", "example.com",
    "test.org", "mail.net", "sample.io", "dev.com",
]

# ─── 생성 함수 ─────────────────────────────────────────────────────────────────

def generate_name_ko() -> tuple:
    """한국어 이름 생성 → (전체 이름, 로마자)"""
    last = random.choice(KO_LAST_NAMES)
    first = random.choice(KO_FIRST_NAMES)
    full = last + first
    roman = KO_LAST_ROMAN.get(last, last) + KO_FIRST_ROMAN.get(first, first)
    return full, roman


def generate_name_en() -> tuple:
    """영어 이름 생성 → (전체 이름, 이메일용 키)"""
    first = random.choice(EN_FIRST_NAMES)
    last = random.choice(EN_LAST_NAMES)
    full = f"{first} {last}"
    roman = f"{first.lower()}.{last.lower()}"
    return full, roman


def generate_name(lang: str) -> tuple:
    if lang == "ko":
        return generate_name_ko()
    if lang == "en":
        return generate_name_en()
    # mixed: 한국어 60%, 영어 40%
    return generate_name_ko() if random.random() < 0.6 else generate_name_en()


def generate_email(roman: str, used: set) -> str:
    """중복 없는 이메일 생성"""
    domain = random.choice(EMAIL_DOMAINS)
    base = roman.replace(" ", ".")
    candidate = f"{base}@{domain}"
    if candidate not in used:
        return candidate
    for i in range(1, 9999):
        candidate = f"{base}{i}@{domain}"
        if candidate not in used:
            return candidate
    # 최후의 수단: 타임스탬프
    ts = int(datetime.now().timestamp() * 1000)
    return f"{base}{ts}@{domain}"


def generate_users(count: int, start_id: int, lang: str, used_emails: set) -> list:
    """더미 사용자 리스트 생성"""
    users = []
    for i in range(count):
        name, roman = generate_name(lang)
        email = generate_email(roman, used_emails)
        used_emails.add(email)
        users.append({"id": start_id + i, "name": name, "email": email})
    return users


# ─── 파일 I/O ──────────────────────────────────────────────────────────────────

def load_db(path: str) -> tuple:
    """기존 users.json 로드 → (users, nextId)"""
    if not os.path.exists(path):
        return [], 1
    try:
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        return data.get("users", []), data.get("nextId", 1)
    except (json.JSONDecodeError, IOError) as e:
        print(f"[WARN] 기존 파일 파싱 실패 ({e}) → 새로 시작합니다.", file=sys.stderr)
        return [], 1


def save_db(path: str, users: list, next_id: int) -> None:
    """users.json 형식으로 저장"""
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    data = {"nextId": next_id, "users": users}
    with open(path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)


# ─── CLI ───────────────────────────────────────────────────────────────────────

def print_table(users: list, label: str = "생성된 사용자") -> None:
    print(f"\n[{label}]")
    print(f"{'ID':>5}  {'이름':<22}  이메일")
    print("─" * 65)
    for u in users:
        print(f"{u['id']:>5}  {u['name']:<22}  {u['email']}")


def main():
    # 기본 출력 경로: 이 스크립트 기준으로 ../DataPersistence/build/users.json
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_output = os.path.normpath(
        os.path.join(script_dir, "..", "DataPersistence", "build", "users.json")
    )

    parser = argparse.ArgumentParser(
        prog="dummy_generator",
        description="DataPersistence.exe 용 더미 사용자 데이터 생성기",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
사용 예시:
  python dummy_generator.py                         기본 10개 생성 (기존 데이터 교체)
  python dummy_generator.py -n 50                   50개 생성
  python dummy_generator.py -n 20 --append          기존 데이터에 20개 추가
  python dummy_generator.py -n 30 --lang en         영어 이름 30개
  python dummy_generator.py -n 100 --seed 42        시드 고정으로 재현 가능
  python dummy_generator.py --preview               저장 없이 미리보기
  python dummy_generator.py -o ./test.json          다른 파일에 저장
        """,
    )
    parser.add_argument("-n", "--count",  type=int, default=10,
                        help="생성할 사용자 수 (기본: 10)")
    parser.add_argument("-o", "--output", default=default_output,
                        help="출력 파일 경로 (기본: ../DataPersistence/build/users.json)")
    parser.add_argument("--append",  action="store_true",
                        help="기존 데이터에 추가 (미지정 시 덮어쓰기)")
    parser.add_argument("--lang",    choices=["ko", "en", "mixed"], default="mixed",
                        help="이름 언어: ko(한국어) / en(영어) / mixed(혼합, 기본)")
    parser.add_argument("--seed",    type=int, default=None,
                        help="랜덤 시드 (재현 가능한 데이터 생성)")
    parser.add_argument("--preview", action="store_true",
                        help="파일 저장 없이 생성 결과만 출력")

    args = parser.parse_args()

    if args.count <= 0:
        parser.error("--count 는 1 이상이어야 합니다.")

    if args.seed is not None:
        random.seed(args.seed)
        print(f"[INFO] 랜덤 시드: {args.seed}")

    output_path = os.path.normpath(args.output)

    # 기존 데이터 로드
    if args.append and not args.preview:
        existing_users, next_id = load_db(output_path)
        used_emails = {u["email"] for u in existing_users}
        print(f"[INFO] 기존 사용자: {len(existing_users)}명  (nextId={next_id})")
    else:
        existing_users, next_id, used_emails = [], 1, set()

    # 더미 데이터 생성
    print(f"[INFO] 더미 사용자 {args.count}명 생성 중... (언어={args.lang})")
    new_users = generate_users(args.count, next_id, args.lang, used_emails)
    all_users = existing_users + new_users
    final_next_id = next_id + args.count

    # 미리보기 또는 저장
    if args.preview:
        print("\n[PREVIEW] 생성될 데이터 (저장 안 함):")
        print(json.dumps(
            {"nextId": final_next_id, "users": new_users},
            indent=2, ensure_ascii=False
        ))
    else:
        try:
            save_db(output_path, all_users, final_next_id)
        except IOError as e:
            print(f"[ERROR] 저장 실패: {e}", file=sys.stderr)
            sys.exit(1)

        print(f"[OK] 저장 완료  →  {output_path}")
        print(f"[OK] 전체 사용자: {len(all_users)}명  (신규: {args.count}명,  nextId: {final_next_id})")
        print_table(new_users, "신규 생성된 사용자")


if __name__ == "__main__":
    main()
