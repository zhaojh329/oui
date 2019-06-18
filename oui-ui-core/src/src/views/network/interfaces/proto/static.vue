<template>
  <div v-if="proto === 'static'">
    <uci-option tab="general" type="input" :label="$t('IPv4 address')" name="ipaddr" depend="proto == 'static'" required rules="ip4addr" @change="ipChange"></uci-option>
    <uci-option tab="general" type="list" :label="$t('IPv4 netmask')" name="netmask" allow-create :options="netmasks" depend="proto == 'static'" required rules="netmask4" @change="maskChange"></uci-option>
    <uci-option tab="general" type="input" :label="$t('IPv4 broadcast')" name="broadcast" :placeholder="broadcast" depend="proto == 'static'" rules="ip4addr"></uci-option>
    <uci-option tab="general" type="input" :label="$t('IPv4 gateway')" name="gateway" depend="proto == 'static'" rules="ip4addr"></uci-option>
    <uci-option tab="general" type="dlist" :label="$t('DNS servers')" name="dns" depend="proto == 'static'" rules="ipaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MAC address')" name="macaddr" :placeholder="macaddr" depend="proto == 'static'" rules="macaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MTU')" name="mtu" placeholder="1500" depend="proto == 'static'" :rules="{type: 'uinteger', max: 9200}"></uci-option>
  </div>
</template>

<script>
export default {
  name: 'OuiProtoStatic',
  inject: ['uciSection'],
  props: {
    proto: String
  },
  data() {
    return {
      ipaddr: '',
      netmask: '',
      netmasks: ['255.255.255.0', '255.255.0.0', '255.0.0.0'],
      broadcast: '',
      macaddr: ''
    }
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
