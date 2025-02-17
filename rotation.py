import cv2
import numpy as np
import os

# Загрузка изображения
image_path = "C:\\Users\\elise\\Downloads\\olimp\\PSRS2425-CompTech-BAK-250203-6 (1)_page-0001.jpg"


image = cv2.imread(image_path)

# Проверка загрузки изображения
if image is None:
    print("Не удалось загрузить изображение. Проверьте путь к файлу.")
    exit()

# Преобразование изображения в серый цвет
gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Применение гауссового размытия для уменьшения шума
blurred_image = cv2.GaussianBlur(gray_image, (5, 5), 0)

# Детектирование границ с помощью Canny
edges = cv2.Canny(blurred_image, 50, 150, apertureSize=3)

# Применение преобразования Хафа для нахождения прямых
lines = cv2.HoughLines(edges, 1, np.pi / 180, 200)

# Если прямые найдены, вычисляем угол наклона
if lines is not None:
    angles = []
    for line in lines:
        rho, theta = line[0]
        angle = np.degrees(theta) - 90  # Преобразование угла в градусы
        if angle < -45:
            angle += 180
        angles.append(angle)

    # Среднее значение углов
    angle = np.mean(angles)
    print(f"Найденный угол наклона: {angle} градусов")
else:
    print("Не удалось найти прямые на изображении.")
    exit()

# Поворот изображения на найденный угол
height, width = image.shape[:2]
rotation_matrix = cv2.getRotationMatrix2D((width / 2, height / 2), angle, 1)
rotated_image = cv2.warpAffine(image, rotation_matrix, (width, height))

# Показываем результат
cv2.imshow('Rotated Image', rotated_image)
cv2.waitKey(0)
cv2.destroyAllWindows()

# Сохранение изображения в ту же папку
directory, filename = os.path.split(image_path)
output_path = os.path.join(directory, 'corrected_image.jpg')
cv2.imwrite(output_path, rotated_image)
print(f"Изображение сохранено как: {output_path}")
