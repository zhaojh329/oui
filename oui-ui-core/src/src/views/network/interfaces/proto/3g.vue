<template>
  <div v-if="proto === '3g'">
    <uci-option tab="general" type="list" :label="$t('Modem device')" name="device" :options="modemDevices" depend="proto == '3g'"></uci-option>
    <uci-option tab="general" type="list" :label="$t('Service Type')" name="service_3g" uci-option="service" :options="services" required depend="proto == '3g'"></uci-option>
    <uci-option tab="general" type="input" label="APN" name="apn" depend="proto == '3g'"></uci-option>
    <uci-option tab="general" type="input" label="PIN" name="pincode" depend="proto == '3g'"></uci-option>
    <uci-option tab="general" type="input" :label="$t('PAP/CHAP username')" name="username" depend="proto == '3g'"></uci-option>
    <uci-option tab="general" type="input" :label="$t('PAP/CHAP password')" name="password" password depend="proto == '3g'"></uci-option>
    <uci-option tab="general" type="input" :label="$t('Dial number')" name="dialnumber" depend="proto == '3g'" placeholder="*99***1#"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Modem init timeout')" name="maxwait" depend="proto == '3g'" placeholder="20" :rules="{type: 'integer', min: 1}"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use default gateway')" name="defaultroute" initial="1" depend="proto == '3g'"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Use gateway metric')" name="metric" placeholder="0" depend="proto == '3g' && defaultroute" rules="uinteger"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" depend="proto == '3g'"></uci-option>
    <uci-option tab="advanced" type="dlist" :label="$t('Use custom DNS servers')" name="dns" depend="proto == '3g' && !peerdns" rules="ipaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('LCP echo failure threshold')" name="_keepalive_failure" placeholder="0" depend="proto == '3g'" rules="uinteger" :load="loadKF" :save="saveKeepalive" @change="onKFChange"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('LCP echo interval')" name="_keepalive_interval" placeholder="5" depend="proto == '3g'" :rules="{type: 'uinteger', min: 1}" :load="loadKI" :save="saveKeepalive" @change="onKIChange"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Inactivity timeout')" name="demand" placeholder="0" depend="proto == '3g'" rules="uinteger"></uci-option>
  </div>
</template>

<script>
export default {
  name: 'OuiProto3g',
  props: {
    proto: String
  },
  data() {
    return {
      modemDevices: [],
      services: [
        ['umts', 'UMTS/GPRS'],
        ['umts_only', this.$t('UMTS only')],
        ['gprs_only', this.$t('GPRS only')],
        ['evdo', 'CDMA/EV-DO']
      ],
      keepalive: {
        failure: '',
        interval: ''
      }
    }
  },
  methods: {
    onKFChange(v) {
      this.keepalive.failure = v;
    },
    onKIChange(v) {
      this.keepalive.interval = v;
    },
    loadKF(resolve, sid) {
      const [v] = (this.$uci.get('network', sid, 'keepalive') || '').split(' ');
      resolve(v);
    },
    loadKI(resolve, sid) {
      const v = (this.$uci.get('network', sid, 'keepalive') || '').split(' ');
      resolve(v[1] || '');
    },
    saveKeepalive(config, sid) {
      if (!this.keepalive.failure && !this.keepalive.interval)
        return;

      const failure = this.keepalive.failure || 0;
      const interval = this.keepalive.interval || 5;
      this.$uci.set('network', sid, 'keepalive', `${failure} ${interval}`);
    }
  },
  created() {
    this.$ubus.call('oui.network', 'modem_list').then(r => {
      this.modemDevices = r.devices;
    });
  }
}
</script>
