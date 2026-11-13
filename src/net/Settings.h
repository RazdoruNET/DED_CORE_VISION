#ifndef NET_SETTINGS_H
#define NET_SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Настройки модуля в NVS: к какому ядру подключаться и пароль обновления.
//
// Пароль точки доступа ядра больше не зашит в прошивку DED CORE V4: он уникален
// для каждого блока, поэтому модуль хранит его у себя и получает при сопряжении
// (см. net/PairingLink.h).
class Settings {
public:
  // Строки настроек пишет loop(), а читает ещё и задача портала. Геттеры
  // отдают ссылки, поэтому мьютекс держит вызывающий: страница портала берёт
  // его на всё время сборки, методы записи — на время записи.
  class Lock {
  public:
    Lock();
    ~Lock();

    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;

  private:
    bool taken_ = false;
  };

  static Settings &instance();

  bool begin();

  bool provisioned() const;
  const String &ssid() const { return ssid_; }
  const String &password() const { return password_; }
  const String &host() const { return host_; }
  uint16_t port() const { return port_; }

  // Токен доверенного модуля: выдаётся при сопряжении, по нему ядро присылает
  // новые параметры сети при смене пароля точки доступа.
  const String &token() const { return token_; }
  void saveToken(const String &token);

  // BSSID точки сопряжения ядра. По нему автоматическое пересопряжение находит
  // только своё ядро, а не любую сеть DED_PAIR_* в радиусе.
  const String &bssid() const { return bssid_; }
  void saveBssid(const String &bssid);

  // Код сопряжения из приложения блока. Сеть DED_PAIR_* закрыта WPA2-паролем,
  // выведенным из этого кода: без него модуль не подключится и не получит пароль ядра.
  const String &pairCode() const { return pairCode_; }
  void savePairCode(const String &code);

  void saveCore(const String &ssid, const String &password, const String &host, uint16_t port);
  void clearCore();

  uint8_t failedBoots() const { return failedBoots_; }
  void setFailedBoots(uint8_t value);

  const String &otaPassword() const { return otaPassword_; }

  // Пароль точки доступа портала: маршруты настройки не авторизуют запрос,
  // поэтому сеть закрыта паролем, который виден на дисплее модуля.
  const String &portalPassword() const { return portalPassword_; }

private:
  Settings() = default;

  void load();
  static String makePassword();

  Preferences prefs_;
  SemaphoreHandle_t mutex_ = nullptr;

  String ssid_;
  String password_;
  String host_;
  uint16_t port_ = 0;
  String token_;
  String bssid_;
  String pairCode_;
  String otaPassword_;
  String portalPassword_;

  // Кэш счётчика: NVS пишется только при изменении значения, иначе «мигающий»
  // линк изнашивал бы flash на каждом переходе в online.
  uint8_t failedBoots_ = 0;
};

#endif // NET_SETTINGS_H
