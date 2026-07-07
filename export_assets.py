import gigaam
import torch
import numpy as np
import json
from omegaconf import OmegaConf, ListConfig, DictConfig

print("Загрузка модели...")
model = gigaam.load_model("ctc")
model.eval()

# 1. Сохраняем словарь (маппинг ID символа -> символ)
try:
    vocab = model.cfg.decoding.vocabulary
    
    # Конвертируем OmegaConf (ListConfig/DictConfig) в обычный Python-объект
    if isinstance(vocab, (ListConfig, DictConfig)):
        vocab = OmegaConf.to_container(vocab, resolve=True)
    elif not isinstance(vocab, (list, dict)):
        vocab = list(vocab) # На случай, если это другой итерируемый объект
        
    with open("vocab.json", "w", encoding="utf-8") as f:
        json.dump(vocab, f, ensure_ascii=False, indent=2)
    print("Словарь сохранен в vocab.json")
except Exception as e:
    print(f"Не удалось сохранить словарь автоматически: {e}")

    # 2. Сохраняем Mel-фильтры (матрицу) - ГАРАНТИРОВАННЫЙ СПОСОБ
    try:
        mel_fb = None
        # Ищем тензор размерностью (80, 257) - это и есть Mel-матрица
        for name, param in model.named_parameters():
            if param.shape[0] == 80 and param.shape[1] == 257:
                mel_fb = param.detach().cpu().numpy()
                print(f"Найдены Mel-фильтры в параметре: {name}, размер: {param.shape}")
                break
        
        if mel_fb is None:
            # На случай, если размер FFT другой (например, 400), будет (80, 201)
            for name, param in model.named_parameters():
                if param.shape[0] == 80 and len(param.shape) == 2:
                    mel_fb = param.detach().cpu().numpy()
                    print(f"Найдены Mel-фильтры (fallback) в параметре: {name}, размер: {param.shape}")
                    break

        if mel_fb is not None:
            np.savetxt("mel_filters.txt", mel_fb)
            print(f"Mel-фильтры сохранены в mel_filters.txt (формат: {mel_fb.shape})")
        else:
            print("Не удалось найти Mel-фильтры автоматически.")
    except Exception as e:
        print(f"Ошибка извлечения mel-фильтров: {e}")

