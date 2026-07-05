import gigaam
import torch

# Загружаем модель RNNT (она скачается автоматически)
model = gigaam.load_model("rnnt")

# Переводим в режим инференса
model.eval()

# Создаем фиктивный вход для RNNT (например, 1 секунда аудио)
# GigaAM использует 16kHz, значит 1 секунда = 16000 сэмплов.
dummy_audio = torch.randn(1, 16000)

# Экспортируем в ONNX
# (Внимание: в зависимости от версии gigaam API может немного отличаться, 
# но обычно у PyTorch моделей есть метод .to_onnx или используем torch.onnx.export)
try:
    # Пробуем стандартный метод экспорта
    torch.onnx.export(
        model,
        (dummy_audio,),
        "gigaam_v3.onnx",
        opset_version=14,
        input_names=["audio"],
        output_names=["tokens"]
    )
    print("Модель успешно экспортирована в gigaam_v3.onnx")
except Exception as e:
    print(f"Ошибка экспорта: {e}")
    print("Возможно, у GigaAM есть свой метод экспорта, смотрите их README.")
