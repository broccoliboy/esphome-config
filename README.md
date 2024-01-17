# ESPHome Configs

## Directory structure

* `hardware`
    * Platform and board base configs
* `common`
    * Configurations shared across devices
* `components`
    * Sharable components that can be included in external projects through `external_components` config. See `devices/gosund-sw2/gosund-sw2.yaml` for example usage.
* `devices`
    * Configs for device classes. One for each part number or type of device that shares the same config.
    * Files with `-import` suffix are used by ESPHome dashboard when the device is adopted.
* `templates`
    * Configs designed to be flashed to devices to enable adoption into esphome dashboards
    * These templates configure devices to use MAC suffix to make each one unique
* `scripts`
    * Helper scripts

## Secrets

The following variables are required in your `secrets.yaml`:

* `wifi_ssid` - SSID of your primary wifi network
* `wifi_password` - Password of your primary wifi network
* `wifi_ssid_fallback` - SSID of a fallback wifi network
* `wifi_password_fallback` - Password of a fallback wifi network
* `api_key` - API key (https://esphome.io/components/api.html)
* `ota_password` - OTA update password
* `web_username` - Web interface username
* `web_password` - Web interface password
* `latitude` - Latitude of device location (used to calculate sunrise/sunset in some devices)
* `longitude` - Longitude of device location
