<template>
  <div>
    <uci-option-list tab="general" :label="$t('Modem device')" name="device" :options="modemDevices"></uci-option-list>
    <uci-option-list tab="general" :label="$t('Service Type')" name="service_3g" uci-option="service" :options="services" required></uci-option-list>
    <uci-option-input tab="general" label="APN" name="apn"></uci-option-input>
    <uci-option-input tab="general" label="PIN" name="pincode"></uci-option-input>
    <uci-option-input tab="general" :label="$t('PAP/CHAP username')" name="username"></uci-option-input>
    <uci-option-input tab="general" :label="$t('PAP/CHAP password')" name="password" password></uci-option-input>
    <uci-option-input tab="general" :label="$t('Dial number')" name="dialnumber" placeholder="*99***1#"></uci-option-input>
    <uci-option-input tab="advanced" :label="$t('Modem init timeout')" name="maxwait" placeholder="20" :rules="{type: 'integer', min: 1}"></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use default gateway')" name="defaultroute" initial="1" ></uci-option-switch>
    <uci-option-input tab="advanced" :label="$t('Use gateway metric')" name="metric" placeholder="0" depend="defaultroute" rules="uinteger"></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" ></uci-option-switch>
    <uci-option-dlist tab="advanced" :label="$t('Use custom DNS servers')" name="dns" depend="!peerdns" rules="ipaddr"></uci-option-dlist>
    <lcp-keepalive></lcp-keepalive>
    <uci-option-input tab="advanced" :label="$t('Inactivity timeout')" name="demand" placeholder="0" rules="uinteger"></uci-option-input>
  </div>
</template>

<script>
import mixin from './proto'
import LcpKeepalive from './lcp-keepalive'

export default {
  mixins: [mixin],
  data() {
    return {
      virtual: true,
      floating: true,
      modemDevices: [],
      services: [
        ['umts', 'UMTS/GPRS'],
        ['umts_only', this.$t('UMTS only')],
        ['gprs_only', this.$t('GPRS only')],
        ['evdo', 'CDMA/EV-DO']
      ]
    }
  },
  components: {
    LcpKeepalive
  },
  created() {
    this.$ubus.call('oui.network', 'modem_list').then(r => {
      this.modemDevices = r.devices;
    });
  }
}
</script>
