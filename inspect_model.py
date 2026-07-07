import gigaam
import torch

print("Загрузка модели...")
model = gigaam.load_model("ctc")
model.eval()

# Делаем фейковое аудио 1 секунда
dummy_audio = torch.randn(1, 16000)
dummy_len = torch.tensor([16000], dtype=torch.int64)

print("\n=== АНАЛИЗ АРХИТЕКТУРЫ GIGAAM ===")
with torch.no_grad():
    # 1. Смотрим, что выдает препроцессор
    mel, mel_len = model.preprocessor(dummy_audio, dummy_len)
    print(f"[1] Выход препроцессора (mel): shape={mel.shape}, dtype={mel.dtype}")

    # 2. Смотрим, что выдает энкодер
    enc, enc_len = model.encoder(mel, mel_len)
    print(f"[2] Выход энкодера (enc): shape={enc.shape}, dtype={enc.dtype}")

print("\n=== ПОИСК CTC-ГОЛОВЫ (Декодера) ===")
# Перечисляем все модули верхнего уровня
for name, module in model.named_children():
    print(f"Модуль: {name} | Тип: {type(module).__name__}")

print("\n=== ПОИСК ЛИНЕЙНОГО СЛОЯ (ВЕСА) ===")
# Ищем линейный слой, размер входа которого совпадает с выходом энкодера
for name, param in model.named_parameters():
    if "weight" in name and param.ndim == 2:
        if param.shape[1] == enc.shape[-1] or param.shape[0] == enc.shape[-1]:
            print(f"Найден кандидат на CTC-голову: {name} | shape={param.shape}")
