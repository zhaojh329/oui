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
          <strong>{{ $t('Uptime') }}</strong>: {{ row.isUp() ? '%t'.format(row.getUptime()) : $t('Interface is down') }}<br/>
          <strong>MAC</strong>: {{ row.getDevice() ? row.getDevice().macaddr : '' }}<br/>
          <strong>RX</strong>: {{ '%mB'.format(row.getStatistics().rx_bytes) }}<br/>
          <strong>TX</strong>: {{ '%mB'.format(row.getStatistics().tx_bytes) }}<br/>
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
    <el-button type="primary" size="small" style="margin-top: 10px" @click="handleAdd">+ {{ $t('Add interface') }}</el-button>
    <el-dialog :title="dialogTitle" :visible.sync="dialogVisible" custom-class="interface-edit-dialog">
      <uci-form config="network" v-if="dialogVisible">
        <uci-section :name="editorIface">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option type="switch" :label="$t('Start on boot')" name="auto" initial="1"></uci-option>
            <uci-option type="list" :label="$t('Protocol')" name="proto" :options="protocols" initial="none" required @change="onProtoChange"></uci-option>
          </uci-tab>
          <uci-tab :title="$t('Advanced Settings')" name="advanced"></uci-tab>
          <uci-tab title="IPv6" name="ipv6"></uci-tab>
          <uci-tab :title="$t('Physical Settings')" name="physical"></uci-tab>
          <uci-tab :title="$t('Firewall Settings')" name="firewall"></uci-tab>
          <oui-proto-dhcp :proto="proto"></oui-proto-dhcp>
          <oui-proto-static :proto="proto"></oui-proto-static>
          <oui-proto-pppoe :proto="proto"></oui-proto-pppoe>
          <oui-proto-pptp :proto="proto"></oui-proto-pptp>
          <oui-proto-l2tp :proto="proto"></oui-proto-l2tp>
          <oui-proto-3g :proto="proto"></oui-proto-3g>
        </uci-section>
      </uci-form>
    </el-dialog>
  </div>
</template>

<script>
import OuiNetworkBadge from './oui-network-badge.vue'
import OuiProtoDhcp from './proto/dhcp.vue'
import OuiProtoStatic from './proto/static.vue'
import OuiProtoPppoe from './proto/pppoe.vue'
import OuiProtoPptp from './proto/pptp.vue'
import OuiProtoL2tp from './proto/l2tp.vue'
import OuiProto3g from './proto/3g.vue'

export default {
  data() {
    return {
      proto: '',
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
      ]
    }
  },
  components: {
    OuiNetworkBadge,
    OuiProtoDhcp,
    OuiProtoStatic,
    OuiProtoPppoe,
    OuiProtoPptp,
    OuiProtoL2tp,
    OuiProto3g
  },
  computed: {
    dialogTitle() {
      return `${this.$t('Configure')} "${this.editorIface}"`
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
    onProtoChange(proto) {
      this.proto = proto;
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
        const loading = this.$getLoading();

        this.$uci.del('network', name);
        this.$uci.save().then(() => {
          this.$uci.apply().then(() => {
            this.load();
            loading.close();
          });
        });
      });
    },
    add(name) {
      const loading = this.$getLoading();

      this.$uci.add('network', 'interface', name);
      this.$uci.save().then(() => {
        this.$uci.apply().then(() => {
          this.load();
          loading.close();
        });
      });
    },
    handleAdd() {
      this.$prompt(this.$t('Please input a name'), this.$t('Add'), {
        inputValidator: value => {
          if (!value || value.match(/^[a-zA-Z0-9_]+$/) === null)
            return this.$t('Must be a valid UCI identifier');

          for (let i = 0; i < this.interfaces.length; i++)
            if (this.interfaces[i].name === value)
              return this.$t('Name already used');

          return true;
        }
      }).then(r => {
        this.add(r.value);
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
