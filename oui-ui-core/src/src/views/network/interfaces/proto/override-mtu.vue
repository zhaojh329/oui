<template>
  <uci-option-input tab="advanced" :label="$t('Override MTU')" name="mtu" placeholder="1500" :rules="{type: 'uinteger', max: 9200}"></uci-option-input>
</template>
