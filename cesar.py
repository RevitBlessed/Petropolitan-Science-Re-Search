def caesar_decode(text, shift, alphabet_ru=None, alphabet_en=None):
    if alphabet_ru is None:
        alphabet_ru = 'абвгдеёжзийклмнопрстуфхцчшщъыьэюя'
    if alphabet_en is None:
        alphabet_en = 'abcdefghijklmnopqrstuvwxyz'
    
    decoded = ''
    for char in text:
        if char.lower() in alphabet_ru:
            idx = alphabet_ru.index(char.lower())
            new_idx = (idx - shift) % len(alphabet_ru)
            decoded += alphabet_ru[new_idx] if char.islower() else alphabet_ru[new_idx].upper()
        elif char.lower() in alphabet_en:
            idx = alphabet_en.index(char.lower())
            new_idx = (idx - shift) % len(alphabet_en)
            decoded += alphabet_en[new_idx] if char.islower() else alphabet_en[new_idx].upper()
        else:
            decoded += char
    return decoded

def detect_shift_by_pattern(text):
    # Определение сдвига по известным паттернам
    known_email_endings = ['.com', '.net', '.org', '.biz']  # Добавлен .biz
    known_address_patterns = ['кв.', 'ул.', 'пр.']
    
    # Проверяем окончания email
    if '@' in text:
        for shift in range(26):
            decoded = caesar_decode(text.lower(), shift)
            for ending in known_email_endings:
                if ending in decoded:
                    # Дополнительная проверка корректности дешифровки
                    if len(decoded.split('@')) == 2 and all(c.isalnum() or c in '.-_@' for c in decoded):
                        return shift
    
    # Проверяем адресные паттерны
    for shift in range(33):  # для русского алфавита
        decoded = caesar_decode(text.lower(), shift)
        for pattern in known_address_patterns:
            if pattern in decoded:
                return shift
    
    return None

def process_data(input_data):
    # Разделяем входные данные на строки
    lines = input_data.strip().split('\n')
    results = []
    
    for line in lines:
        if line.strip():
            shift = detect_shift_by_pattern(line)
            if shift is not None:
                decoded = caesar_decode(line, shift)
                results.append({
                    'original': line,
                    'decoded': decoded,
                    'shift': shift
                })
    
    return results

# Основная обработка данных
input_text = """
pmjmz.bzmujtig@nzqmamv.kwu

giyh.lsxyl@pifeguh.vct

enxbepbms.tkexgx@ykbxlxg.vhf

eobh.xypyh@zyyhys.wig

kizumtw36@rwpvawv.kwu

nevvix.kpiewsr@lsxqemp.gsq

pexkxexk.biblklob@elqjxfi.zlj

ugyvp@wbkj.xod

trwxg.gbxghp@tulabkx.ubs

tdquvjw.wjxvrn@hjqxx.lxv

ыу. Лпмшорхщтцкцл.6 тй.466

Юдуебвудашэеюут жя.ч.34 юх.102

вг. Сэёкэыатц.60 эф.107

Ф. Аудзьбеюут жя.ч.31 юх.159

ыу. Игщъшцунъцйзл.81 тй.107

Ёяхтодг чп.з.77 оё.232

Дешезйпьшзбцх йв.ъ.80 бш.422

эх. Нйбчйин.25 фл.43

Дебюубфтс ёю.ц.14 эф.61

Ъщ. Усъфчкъуст шнщ.м.73 ук.65
"""

results = process_data(input_text)

# Вывод результатов
print("\nРезультаты дешифровки:")
print("-" * 50)
for result in results:
    print(f"Исходный текст: {result['original']}")
    print(f"Дешифрованный текст: {result['decoded']}")
    print(f"Ключ шифрования: {result['shift']}")
    print("-" * 50)

# Сохранение результатов в файл с дополнительным столбцом для ключа шифрования
with open('deobfuscated_data.txt', 'w', encoding='utf-8') as f:
    f.write("Исходный текст | Дешифрованный текст | Ключ шифрования\n")
    f.write("-" * 80 + "\n")
    for result in results:
        f.write(f"{result['original']} | {result['decoded']} | {result['shift']}\n")

