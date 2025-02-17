class CaesarCipher:
    """
    Класс для работы с шифром Цезаря.
    Поддерживает русский и английский алфавиты.
    """
    def __init__(self):
        self.ru_alphabet = 'абвгдеёжзийклмнопрстуфхцчшщъыьэюя'
        self.en_alphabet = 'abcdefghijklmnopqrstuvwxyz'
        
    def detect_alphabet(self, text):
        """Определяет используемый алфавит"""
        first_letter = next((c.lower() for c in text if c.isalpha()), None)
        if first_letter:
            return self.ru_alphabet if 'а' <= first_letter <= 'я' else self.en_alphabet
        return None

    def find_shift(self, encrypted, decrypted):
        """
        Находит сдвиг на основе пары зашифрованный-расшифрованный текст.
        Возвращает (сдвиг, алфавит).
        """
        if len(encrypted) != len(decrypted):
            return None, None
            
        alphabet = self.detect_alphabet(encrypted)
        if not alphabet:
            return None, None
            
        for e, d in zip(encrypted.lower(), decrypted.lower()):
            if e in alphabet and d in alphabet:
                e_idx = alphabet.index(e)
                d_idx = alphabet.index(d)
                shift = (e_idx - d_idx) % len(alphabet)
                return shift, alphabet
        return None, None

    def decrypt_with_shift(self, text, shift, alphabet):
        """Дешифрует текст с заданным сдвигом"""
        result = ''
        for char in text:
            if char.lower() in alphabet:
                idx = alphabet.index(char.lower())
                new_idx = (idx - shift) % len(alphabet)
                new_char = alphabet[new_idx]
                result += new_char.upper() if char.isupper() else new_char
            else:
                result += char
        return result

def decrypt_text(text, known_pairs=None):
    """
    Дешифрует текст, используя известные пары зашифрованный-расшифрованный текст.
    Возвращает (расшифрованный текст, использованный сдвиг).
    """
    cipher = CaesarCipher()
    
    # Объединяем известные паттерны для русских адресов и английских email-ов
    patterns = {
        # Русские адреса
        'тй': 'кв', 'юх': 'кв', 'эф': 'кв', 'бш': 'кв',
        'ук': 'кв', 'фл': 'кв', 'оё': 'кв',
        # Английские email-паттерны (например, доменные окончания)
        'kwu': 'com',   # например, "nzqmamv.kwu" -> "gmail.com" или "rwpvawv.kwu" -> "privacy.com"
        'xod': 'net',   # например, "wbkj.xod" -> "user.net"
        'gsq': 'com'    # например, "lsxqemp.gsq" -> "gmail.com"
    }
    if known_pairs:
        patterns.update(known_pairs)
        
    # Пытаемся найти сдвиг, используя паттерны (для любых текстов: адресов или email-ов)
    for encrypted_pattern, decrypted_pattern in patterns.items():
        if encrypted_pattern in text.lower():
            shift, alphabet = cipher.find_shift(encrypted_pattern, decrypted_pattern)
            if shift is not None and alphabet is not None:
                decrypted = cipher.decrypt_with_shift(text, shift, alphabet)
                return decrypted, shift
                
    # Если подходящий шаблон не найден, возвращаем исходный текст
    return text, None

def process_data(input_data):
    """
    Обрабатывает входные данные и возвращает результаты дешифровки
    с информацией о использованных ключах.
    """
    results = []
    lines = [line.strip() for line in input_data.split('\n')]
    
    # Пропускаем заголовок (предполагается, что первые две строки – заголовок)
    for line in lines[2:]:
        decrypted, shift = decrypt_text(line)
        results.append({
            'original': line,
            'decrypted': decrypted,
            'shift_key': shift if shift is not None else "N/A"
        })
    
    return results

def save_results(results, output_file):
    """Сохраняет результаты в файл"""
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("Исходный текст | Расшифрованный текст | Ключ шифрования\n")
        f.write("-" * 70 + "\n")
        for result in results:
            f.write(f"{result['original']} | {result['decrypted']} | {result['shift_key']}\n")

# Пример использования
if __name__ == "__main__":
    encrypted_data = """email Адрес
pmjmz.bzmujtig@nzqmamv.kwu
giyh.lsxyl@pifeguh.vct
ыу. Лпмшорхщтцкцл.6 тй.466
Юдуебвудашэеюут жя.ч.34 юх.102
enxbepbms.tkexgx@ykbxlxg.vhf
eobh.xypyh@zyyhys.wig
вг. Сэёкэыатц.60 эф.107
Ф. Аудзьбеюут жя.ч.31 юх.159
kizumtw36@rwpvawv.kwu
nevvix.kpiewsr@lsxqemp.gsq
ыу. Игщъшцунъцйзл.81 тй.107
pexkxexk.biblklob@elqjxfi.zlj
ugyvp@wbkj.xod
Ёяхтодг чп.з.77 оё.232
Дешезйпьшзбцх йв.ъ.80 бш.422
эх. Нйбчйин.25 фл.43
trwxg.gbxghp@tulabkx.ubs
tdquvjw.wjxvrn@hjqxx.lxv
Дебюубфтс ёю.ц.14 эф.61
Ъщ. Усъфчкъуст шнщ.м.73 ук.65"""

    results = process_data(encrypted_data)
    
    # Выводим результаты на экран
    print("Результаты дешифровки:")
    for result in results:
        print(f"Исходный текст: {result['original']}")
        print(f"Расшифрованный текст: {result['decrypted']}")
        print(f"Ключ шифрования: {result['shift_key']}")
        print("-" * 50)
    
    # Сохраняем результаты в файл
    save_results(results, "decryption_results.txt")
