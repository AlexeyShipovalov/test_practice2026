import torch
import torchaudio
import numpy as np

print("Генерация ИСПРАВЛЕННЫХ Mel-фильтров...")
mel_transform = torchaudio.transforms.MelSpectrogram(
    sample_rate=16000,
    n_fft=512,
    win_length=512,
    hop_length=160,
    n_mels=64,
    window_fn=torch.hann_window
)

# В Python матрица (257, 64). Транспонируем в (64, 257)!
mel_fb = mel_transform.mel_scale.fb.detach().cpu().numpy()
mel_fb_transposed = mel_fb.T  # <--- ВОТ ГЛАВНОЕ ИСПРАВЛЕНИЕ

print(f"Форма старая: {mel_fb.shape} (257 строк по 64)")
print(f"Форма новая: {mel_fb_transposed.shape} (64 строки по 257 - ПРАВИЛЬНО!)")

np.savetxt("models/mel_filters.txt", mel_fb_transposed)
print("Исправленные mel-фильтры сохранены в models/mel_filters.txt")
