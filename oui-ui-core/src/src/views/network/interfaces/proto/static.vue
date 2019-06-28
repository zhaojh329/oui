<template>
  <div>
    <uci-option-input tab="general" :label="$t('IPv4 address')" name="ipaddr" required rules="ip4addr" @change="ipChange"></uci-option-input>
    <uci-option-list tab="general" :label="$t('IPv4 netmask')" name="netmask" allow-create :options="netmasks" required rules="netmask4" @change="maskChange"></uci-option-list>
    <uci-option-input tab="general" :label="$t('IPv4 broadcast')" name="broadcast" :placeholder="broadcast" rules="ip4addr"></uci-option-input>
    <uci-option-input tab="general" :label="$t('IPv4 gateway')" name="gateway" rules="ip4addr"></uci-option-input>
    <uci-option-dlist tab="general" :label="$t('DNS servers')" name="dns" rules="ipaddr"></uci-option-dlist>
    <override-mac></override-mac>
    <override-mtu></override-mtu>
    <uci-option-input tab="advanced" :label="$t('Override MTU')" name="mtu" placeholder="1500" :rules="{type: 'uinteger', max: 9200}"></uci-option-input>
  </div>
</template>

<script>
import mixin from './proto'
import OverrideMac from './override-mac'
import OverrideMtu from './override-mtu'

export default {
  mixins: [mixin],
  inject: ['uciSection'],
  data() {
    return {
      virtual: false,
      floating: false,
      ipaddr: '',
      netmask: '',
      netmasks: ['255.255.255.0', '255.255.0.0', '255.0.0.0'],
      broadcast: '',
      macaddr: ''
    }
  },
  components: {
    OverrideMac,
    OverrideMtu
  },
  methods: {
    calculateBroadcast() {
      const i = this.ipaddr.split('.');
      const m = this.netmask.split('.');

      let I = 0;
      let M = 0;

      for (let n = 0; n < 4; n++) {
        i[n] = parseInt(i[n]);
        m[n] = parseInt(m[n]);

        if (isNaN(i[n]) || i[n] < 0 || i[n] > 255 ||
          isNaN(m[n]) || m[n] < 0 || m[n] > 255)
          return;

        I |= (i[n] << ((3 - n) * 8));
        M |= (m[n] << ((3 - n) * 8));
      }

      const B = I | ~M;

      this.broadcast = '%d.%d.%d.%d'.format((B >> 24) & 0xFF, (B >> 16) & 0xFF, (B >> 8) & 0xFF, (B >> 0) & 0xFF);
    },
    ipChange(v) {
      this.ipaddr = v;
      this.calculateBroadcast();
    },
    maskChange(v) {
      this.netmask = v;
      this.calculateBroadcast();
    }
  },
  created() {
    const iface = this.$network.getInterface(this.uciSection.name);
    const dev = iface.getDevice();
    if (dev)
      this.macaddr = dev.macaddr;
  }
}
</script>
