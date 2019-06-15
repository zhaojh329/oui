<template>
  <div>
    <el-table :data="interfaces">
      <el-table-column :label="$t('Network')" width="140">
        <template v-slot="{ row }">
          <oui-network-badge :iface="row.name" :device="row.getDevice() && row.getDevice().name"></oui-network-badge>
        </template>
      </el-table-column>
      <el-table-column :label="$t('Status')">
        <template v-slot="{ row }">
          <strong>{{ $t('Uptime') }}</strong>: {{ row.isUp() ? row.getUptime() : $t('Interface is down') }}<br/>
          <strong>MAC</strong>: {{ row.getDevice() ? row.getDevice().macaddr : '' }}<br/>
          <strong>RX</strong>: {{ row.getStatistics().rx_bytes }}<br/>
          <strong>TX</strong>: {{ row.getStatistics().tx_bytes }}<br/>
          <strong>IPv4</strong>: {{ row.getIPv4Addrs().join(',') }}<br/>
          <strong>IPv6</strong>: {{ row.getIPv6Addrs().join(',') }}<br/>
        </template>
      </el-table-column>
      <el-table-column label="#">
        <template v-slot="{ row }">
          <el-button size="mini" @click="ifup(row.name)">{{ $t('Start') }}</el-button>
          <el-button size="mini" @click="ifdown(row.name)">{{ $t('Stop') }}</el-button>
          <el-button type="primary" size="mini" @click="edit(row.name)">{{ $t('Edit') }}</el-button>
          <el-button type="danger" size="mini" @click="del(row.name)">{{ $t('Delete') }}</el-button>
        </template>
      </el-table-column>
    </el-table>
    <el-dialog :title="dialogTitle" :visible.sync="dialogVisible" custom-class="interface-edit-dialog">
      <uci-form config="network">
        <uci-section :name="editorIface">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option type="switch" :label="$t('Start on boot')" name="auto" initial="1"></uci-option>
            <uci-option type="list" :label="$t('Protocol')" name="proto" :options="protocols" initial="none" required></uci-option>
            <uci-option type="input" :label="$t('Hostname')" name="hostname" depend="proto == 'dhcp'" :placeholder="hostname" rules="hostname"></uci-option>
            <uci-option type="input" :label="$t('IPv4 address')" name="ipaddr" depend="proto == 'static'" required rules="ip4addr"></uci-option>
            <uci-option type="input" :label="$t('IPv4 netmask')" name="netmask" depend="proto == 'static'" required rules="netmask4"></uci-option>
            <uci-option type="input" :label="$t('IPv4 broadcast')" name="broadcast" depend="proto == 'static'" rules="ip4addr"></uci-option>
            <uci-option type="input" :label="$t('IPv4 gateway')" name="gateway" depend="proto == 'static'" rules="ip4addr"></uci-option>
            <uci-option type="dlist" :label="$t('DNS servers')" name="dns_static" uci-option="dns" depend="proto == 'static'" rules="ipaddr"></uci-option>
            <uci-option type="input" :label="$t('Server')" name="server" depend="proto == 'pptp' || proto == 'l2tp'" required rules="host"></uci-option>
            <uci-option type="list" :label="$t('Modem device')" name="device" :options="modemDevices" depend="proto == '3g'"></uci-option>
            <uci-option type="list" :label="$t('Service Type')" name="service_3g" uci-option="service" :options="services" required depend="proto == '3g'"></uci-option>
            <uci-option type="input" label="APN" name="apn" depend="proto == '3g'"></uci-option>
            <uci-option type="input" label="PIN" name="pincode" depend="proto == '3g'"></uci-option>
            <uci-option type="input" :label="$t('PAP/CHAP username')" name="username" depend="proto == 'pppoe' || proto == 'pptp' || proto == 'l2tp' || proto == '3g'"></uci-option>
            <uci-option type="input" :label="$t('PAP/CHAP password')" name="password" depend="proto == 'pppoe' || proto == 'pptp' || proto == 'l2tp' || proto == '3g'"></uci-option>
            <uci-option type="input" :label="$t('Access Concentrator')" name="ac" placeholder="auto" depend="proto == 'pppoe'"></uci-option>
            <uci-option type="input" :label="$t('Service Name')" name="service_pppoe" uci-option="service" placeholder="auto" depend="proto == 'pppoe'"></uci-option>
            <uci-option type="input" :label="$t('Dial number')" name="dialnumber" depend="proto == '3g'" placeholder="*99***1#"></uci-option>
          </uci-tab>
          <uci-tab :title="$t('Advanced Settings')" name="advanced">
            <uci-option type="input" :label="$t('Modem init timeout')" name="maxwait" depend="proto == '3g'" placeholder="20" :rules="{type: 'integer', min: 1}"></uci-option>
            <uci-option type="switch" :label="$t('Use broadcast')" name="broadcast" depend="proto == 'dhcp'"></uci-option>
            <uci-option type="switch" :label="$t('Use gateway')" name="defaultroute" initial="1" depend="proto == 'dhcp' || proto == 'pppoe' || proto == 'pptp' || proto == 'l2tp' || proto == '3g'"></uci-option>
            <uci-option type="switch" :label="$t('Use DNS')" name="peerdns" initial="1" depend="proto == 'dhcp' || proto == 'pppoe' || proto == 'pptp' || proto == 'l2tp' || proto == '3g'"></uci-option>
            <uci-option type="dlist" :label="$t('Custom DNS')" name="dns_custom" uci-option="dns" depend="(proto == 'dhcp' || proto == 'pppoe' || proto == 'pptp' || proto == 'l2tp' || proto == '3g') && !peerdns" rules="ipaddr"></uci-option>
            <uci-option type="input" :label="$t('Inactivity timeout')" name="demand" placeholder="0" depend="proto == 'pppoe'" rules="uinteger"></uci-option>
            <uci-option type="input" :label="$t('Client ID')" name="clientid" depend="proto == 'dhcp'"></uci-option>
            <uci-option type="input" :label="$t('Vendor Class')" name="vendorid" depend="proto == 'dhcp'"></uci-option>
          </uci-tab>
          <uci-tab title="IPv6" name="ipv6"></uci-tab>
          <uci-tab :title="$t('Physical Settings')" name="physical"></uci-tab>
          <uci-tab :title="$t('Firewall Settings')" name="firewall"></uci-tab>
        </uci-section>
      </uci-form>
    </el-dialog>
  </div>
</template>

<script>
import OuiNetworkBadge from '@/components/oui-network-badge.vue'

export default {
  data() {
    return {
      interfaces: [],
      dialogVisible: false,
      editorIface: '',
      protocols: [
        ['none', this.$t('Unmanaged')],
        ['dhcp', this.$t('DHCP Client')],
        ['static', this.$t('Static address')],
        ['pppoe', 'PPPoE'],
        ['pptp', 'PPtP'],
        ['l2tp', 'L2TP'],
        ['3g', '3G']
      ],
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
    OuiNetworkBadge
  },
  computed: {
    dialogTitle() {
      return `${this.$t('Configure')} "${this.editorIface}"`
    },
    hostname() {
      return this.$store.state.hostname;
    }
  },
  timers: {
    load: {time: 3000, autostart: true, immediate: true, repeat: true}
  },
  methods: {
    load() {
      this.$network.load().then(() => {
        this.interfaces = this.$network.getInterfaces().filter(i => i.name !== 'loopback');
      });
    },
    edit(iface) {
      this.editorIface = iface;
      this.dialogVisible = true;
    },
    ifup(name) {
      this.$ubus.call('oui.network', 'ifup', {data: name});
    },
    ifdown(name) {
      this.$ubus.call('oui.network', 'ifdown', {data: name});
    },
    del(name) {
      this.$confirm(this.$t('Really delete this interface? The deletion cannot be undone!You might lose access to this device if you are connected via this interface.'), `${this.$t('Delete interface')} "${name}"`).then(() => {
        const loading = this.$loading({
          text: this.$t('loading...'),
          spinner: 'el-icon-loading',
          background: 'rgba(0, 0, 0, 0.7)'
        });

        this.$uci.del('network', name);
        this.$uci.save().then(() => {
          this.$uci.apply().then(() => {
            this.load();
            loading.close();
          });
        });
      });
    }
  },
  created() {
    this.$ubus.call('oui.network', 'modem_list').then(r => {
      this.modemDevices = r.devices;
    });
  }
}
</script>

<style lang="scss">
.interface-edit-dialog {
  .el-dialog__header {
    padding: 10px 20px 10px;
  }
  .el-dialog__body {
    padding: 0;
  }
}
</style>
