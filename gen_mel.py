import torchaudio
import numpy as np
import torch

print("Генерация Mel-фильтров для GigaAM (n_mels=64, n_fft=512)...")
# Создаем объект MelSpectrogram с параметрами GigaAM
mel_transform = torchaudio.transforms.MelSpectrogram(
    sample_rate=16000,
    n_fft=512,
    win_length=512,
    hop_length=160,
    n_mels=64,
    window_fn=torch.hann_window
)

# Извлекаем матрицу фильтров из объекта
mel_fb = mel_transform.mel_scale.fb.detach().cpu().numpy()

print(f"Форма матрицы: {mel_fb.shape}") # Должно быть ровно (64, 257)

np.savetxt("mel_filters.txt", mel_fb)
print("Успех! Сохранено в mel_filters.txt")
