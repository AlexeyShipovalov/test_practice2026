import gigaam
import torch
import torch.nn as nn

print("Загрузка CTC модели...")
model = gigaam.load_model("ctc")
model.eval()

# Создаем пустышку-заглушку, которая просто пропускает через себя данные без STFT
class BypassPreprocessor(nn.Module):
    def forward(self, mel_features, feature_lengths):
        return mel_features, feature_lengths

# Хирургически подменяем проблемный блок!
print("Подмена препроцессора (удаление STFT)...")
model.preprocessor = BypassPreprocessor()

# ВАЖНО: Используем ровно 64 Mel-коэффициента, как показал рентген!
dummy_mel = torch.randn(1, 64, 101)
dummy_mel_len = torch.tensor([101], dtype=torch.int64)

onnx_path = "gigaam_encoder_decoder.onnx"

print("Начинаем экспорт модифицированной модели в ONNX...")
try:
    torch.onnx.export(
        model,                              
        (dummy_mel, dummy_mel_len),
        onnx_path,
        opset_version=14,
        input_names=["mel_features", "feature_lengths"],
        output_names=["logits"],
        dynamic_axes={
            "mel_features": {2: "seq_len"},   
            "feature_lengths": {0: "batch_size"},
            "logits": {1: "seq_len"}
        }
    )
    print(f"УСПЕХ! Модель успешно сохранена в файл: {onnx_path}")
except Exception as e:
    print(f"Ошибка экспорта: {e}")
