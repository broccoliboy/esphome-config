# ESPHome Configs

## Directory structure

Top level directory contains individual node configs and `secrets.yaml`. Any file with `-template` suffix is for initial device flashing so it will be recognized by ESPHome. `secrets_substitutes.yaml` makes it easier to import all secrets in one line.

* `devices`
    * Full config for specific devices
    * Import configs for when devices are adopted into Home Assistant ESPHome
* `hardware`
    * Platform and board configs
* `components`
    * Component configurations (wifi, ota, etc.)

