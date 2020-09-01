<template>
  <oui-form-item-input :uci-section="uciSection" :label="$t('Override MTU')" name="mtu" placeholder="1500" :rules="{type: 'uinteger', max: 9200}"/>
</template>

<script>
export default {
  name: 'OverrideMtu',
  props: {
    uciSection: Object
  }
}
</script>
