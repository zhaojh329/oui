<template>
  <oui-form-item-select :uci-section="uciSection" :label="$t('Protocol')" name="proto" initial="none" :options="protocols"/>
</template>

<script>
export default {
  name: 'Proto',
  props: {
    uciSection: Object
  },
  data () {
    return {
      protocols: [
        ['none', this.$t('Unmanaged')],
        ['dhcp', this.$t('DHCP Client')],
        ['static', this.$t('Static address')],
        ['pppoe', 'PPPoE'],
        ['pptp', 'PPtP'],
        ['l2tp', 'L2TP'],
        ['3g', '3G']
      ]
    }
  }
}
</script>
