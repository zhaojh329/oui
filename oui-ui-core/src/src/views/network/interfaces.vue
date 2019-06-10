<template>
  <div>
    <el-table :data="interfaces">
      <el-table-column prop="interface" label="Network" width="140">
        <template v-slot="{ row }">
          <oui-network-badge :iface="row.name" :device="row.getDevice() && row.getDevice().name"></oui-network-badge>
        </template>
      </el-table-column>
      <el-table-column prop="interface" label="Status">
        <template v-slot="{ row }">
          <strong>Uptime</strong>: {{ row.isUp() ? row.getUptime() : 'Interface is down' }}<br/>
          <strong>MAC</strong>: {{ row.getDevice() ? row.getDevice().macaddr : '' }}<br/>
          <strong>RX</strong>: {{ row.getStatistics().rx_bytes }}<br/>
          <strong>TX</strong>: {{ row.getStatistics().tx_bytes }}<br/>
          <strong>IPv4</strong>: {{ row.getIPv4Addrs().join(',') }}<br/>
          <strong>IPv6</strong>: {{ row.getIPv6Addrs().join(',') }}<br/>
        </template>
      </el-table-column>
      <el-table-column label="#">
        <template v-slot="{ row }">
          <el-button size="mini" @click="ifup(row.name)">Start</el-button>
          <el-button size="mini" @click="ifdown(row.name)">Stop</el-button>
          <el-button type="primary" size="mini" @click="edit(row.name)">Edit</el-button>
          <el-button type="danger" size="mini" @click="del(row.name)">Delete</el-button>
        </template>
      </el-table-column>
    </el-table>
    <el-dialog :title="dialogTitle" :visible.sync="dialogVisible" custom-class="interface-edit-dialog">
      <uci-form config="network">
        <uci-section :name="editorIface">
          <uci-tab title="General Settings" name="general">
            <uci-option type="switch" label="Start on boot" name="auto" initial="1"></uci-option>
            <uci-option type="list" label="Protocol" name="proto" :options="protocols" initial="none" required></uci-option>
            <uci-option type="input" label="IPv4 address" name="ipaddr" depend="proto == 'static'" required></uci-option>
            <uci-option type="input" label="IPv4 netmask" name="netmask" depend="proto == 'static'" required></uci-option>
            <uci-option type="input" label="IPv4 broadcast" name="broadcast" depend="proto == 'static'"></uci-option>
            <uci-option type="input" label="IPv4 gateway" name="gateway" depend="proto == 'static'"></uci-option>
            <uci-option type="dlist" label="DNS servers" name="dns" depend="proto == 'static'"></uci-option>
            <uci-option type="input" label="PAP/CHAP username" name="username" depend="proto == 'pppoe'" required></uci-option>
            <uci-option type="input" label="PAP/CHAP password" name="password" depend="proto == 'pppoe'"></uci-option>
            <uci-option type="input" label="Access Concentrator" name="ac" placeholder="auto" depend="proto == 'pppoe'"></uci-option>
            <uci-option type="input" label="Service Name" name="service" placeholder="auto" depend="proto == 'pppoe'"></uci-option>
          </uci-tab>
          <uci-tab title="Advanced Settings" name="advanced">
            <uci-option type="switch" label="Use gateway" name="defaultroute" initial="1" depend="proto == 'pppoe'"></uci-option>
            <uci-option type="switch" label="Use DNS" name="peerdns" initial="1" depend="proto == 'pppoe'"></uci-option>
            <uci-option type="dlist" label="Custom DNS" name="dns" depend="proto == 'pppoe', peerdns == '0'"></uci-option>
            <uci-option type="input" label="Inactivity timeout" name="demand" placeholder="0" depend="proto == 'pppoe'"></uci-option>
          </uci-tab>
          <uci-tab title="IPv6" name="ipv6"></uci-tab>
          <uci-tab title="Physical Settings" name="physical"></uci-tab>
          <uci-tab title="Firewall Settings" name="firewall"></uci-tab>
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
        ['none', 'Unmanaged'],
        ['dhcp', 'DHCP Client'],
        ['pppoe', 'PPPoE'],
        ['static', 'Static address']
      ]
    }
  },
  components: {
    OuiNetworkBadge
  },
  computed: {
    dialogTitle() {
      return `Configure "${this.editorIface}"`
    }
  },
  timers: {
    load: {time: 3000, autostart: true, immediate: true, repeat: true}
  },
  methods: {
    load() {
      this.$network.load().then(() => {
        this.interfaces = this.$network.getInterfaces();
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
      this.$confirm('Really delete this interface? The deletion cannot be undone!You might lose access to this device if you are connected via this interface.', `Delete interface "${name}"`).then(() => {
        const loading = this.$loading({
          text: 'loading...',
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
