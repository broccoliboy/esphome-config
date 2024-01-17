#!/bin/bash

for template in ${0%/*}/../templates/*-template.yaml; do
    esphome compile ${template}
done
