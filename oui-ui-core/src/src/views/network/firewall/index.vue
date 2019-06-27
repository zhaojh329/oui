<template>
  <uci-form config="firewall" ref="form" tabbed :after-loaded="load">
    <uci-section type="defaults" :title="$t('General Settings')">
      <uci-option-switch :label="$t('Enable SYN-flood protection')" name="syn_flood"></uci-option-switch>
      <uci-option-switch :label="$t('Drop invalid packets')" name="drop_invalid"></uci-option-switch>
      <uci-option-list :label="$t('Input')" name="input" :options="targets" required></uci-option-list>
      <uci-option-list :label="$t('Output')" name="output" :options="targets" required></uci-option-list>
      <uci-option-list :label="$t('Forward')" name="forward" :options="targets" required></uci-option-list>
    </uci-section>
    <zone :zones="zones" :targets="targets"></zone>
    <dnat :zones="zones" :protos="protos"></dnat>
    <rules :zones="zones" :protos="protos"></rules>
  </uci-form>
</template>

<script>
import Zone from './zone'
import Dnat from './dnat'
import Rules from './rules'

export default {
  data() {
    return {
      protos: [
        ['tcp udp', 'TCP+UDP'],
        ['tcp', 'TCP'],
        ['udp', 'UDP'],
        ['icmp', 'ICMP']
      ],
      targets: [
        ['REJECT', this.$t('reject')],
        ['DROP', this.$t('drop')],
        ['ACCEPT', this.$t('accept')]
      ],
      zones: []
    }
  },
  components: {
    Zone,
    Dnat,
    Rules
  },
  methods: {
    load() {
      this.$firewall.load(true);
      this.zones = this.$firewall.zones.map(z => z.name());
    }
  }
}
</script>
