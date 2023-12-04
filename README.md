# ESPHome Configs

## Directory structure

Top level directory contains individual node configs and `secrets.yaml`. Files with `-template` suffix are for initial device flashing so it will be recognized by ESPHome. These files configure devices with MAC suffix so they are unique when adopting into ESPHome/Home Assistant. After adopting the device you must rename it to something unique or manually copy the MAC suffix from the filename (see `devices/gosund-sw2/gosund-sw2-import.yaml` for example).

`secrets_substitutes.yaml` makes it easier to import all secrets in one line.

* `hardware`
    * Platform and board configs
* `common`
    * Configurations shared across devices
* `components`
    * Sharable components that can be included in external projects through `external_components` config. See `devices/gosund-sw2/gosund-sw2.yaml` for example usage.
* `devices`
    * Configs for device classes. One for each part number or type of device that shares the same config.
    * Files with `-import` suffix are used by ESPHome when the device is adopted through the dashboard
* `templates`
    * Configs designed to be flashed to nodes to enable adoption into esphome dashboards
* `nodes`
    * Individual device configs (one for each individual device on the network)
