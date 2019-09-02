<template>
  <div>
    <el-table :data="interfaces">
      <el-table-column :label="$t('Network')" width="140">
        <template v-slot="{ row }">
          <network-badge :iface="row.name" :device="row.getDevice() && row.getDevice().name"></network-badge>
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
      <uci-form config="network" v-if="dialogVisible" :apply-timeout="15">
        <uci-section :name="editorIface">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option-switch :label="$t('Start on boot')" name="auto" initial="1"></uci-option-switch>
            <uci-option-list :label="$t('Protocol')" name="proto" :options="protocols" initial="none" required @change="protoChanged"></uci-option-list>
          </uci-tab>
          <uci-tab :title="$t('Advanced Settings')" name="advanced">
            <uci-option-switch :label="$t('Use builtin IPv6-management')" name="delegate" initial="1"></uci-option-switch>
            <uci-option-switch :label="$t('Force link')" name="force_link" :initial="proto === 'static' ? true : false" :description="$t('Set interface properties regardless of the link carrier (If set, carrier sense events do not invoke hotplug handlers).')"></uci-option-switch>
          </uci-tab>
          <uci-tab :title="$t('Physical Settings')" name="physical">
            <template v-if="!virtual">
              <uci-option-switch :label="$t('Bridge interfaces')" name="type" active-value="bridge" :save="saveType" depend="proto == 'static' || proto == 'dhcp' || proto == 'none'" :description="$t('creates a bridge over specified interface(s)')"></uci-option-switch>
              <uci-option-switch :label="$t('Enable STP')" name="stp" depend="type" :description="$t('Enables the Spanning Tree Protocol on this bridge')"></uci-option-switch>
              <uci-option-switch :label="$t('Enable IGMP')" name="igmp_snooping" depend="type" :description="$t('Enables IGMP snooping on this bridge')"></uci-option-switch>
            </template>
            <ifname v-if="!floating"></ifname>
            <ifname v-if="!virtual" multiple></ifname>
          </uci-tab>
          <uci-tab :title="$t('Firewall Settings')" name="firewall">
            <uci-option-list :label="$t('Create / Assign firewall-zone')" name="_fwzone" :options="zones" :load="loadZone" :save="saveZone" allow-create :description="$t('interface-config-zone-desc')"></uci-option-list>
          </uci-tab>
          <component v-if="proto !== '' && proto !== 'none'" :is="'proto-' + proto" @mounted="onProtoMounted"></component>
        </uci-section>
      </uci-form>
    </el-dialog>
  </div>
</template>

<script>
import NetworkBadge from './network-badge.vue'
import ProtoDhcp from './proto/dhcp.vue'
import ProtoStatic from './proto/static.vue'
import ProtoPppoe from './proto/pppoe.vue'
import ProtoPptp from './proto/pptp.vue'
import ProtoL2tp from './proto/l2tp.vue'
import Proto3g from './proto/3g.vue'
import Ifname from './ifname.vue'

export default {
  data() {
    return {
      proto: '',
      virtual: false,
      floating: false,
      interfaces: [],
      devices: [],
      zones: [],
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
    NetworkBadge,
    ProtoDhcp,
    ProtoStatic,
    ProtoPppoe,
    ProtoPptp,
    ProtoL2tp,
    Proto3g,
    Ifname
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
        this.interfaces = this.$network.getInterfaces();
      });
    },
    protoChanged(proto) {
      this.proto = proto;
    },
    onProtoMounted(proto) {
      this.virtual = proto.virtual;
      this.floating = proto.floating;
    },
    saveType(sid, value) {
      this.$uci.set('network', sid, 'type', value || '');
    },
    loadZone() {
      return new Promise(resolve => {
        this.$firewall.load().then(() => {
          this.zones = this.$firewall.zones.map(z => z.name());
          const z = this.$firewall.findZoneByNetwork(this.editorIface);
          if (z)
            resolve(z.name());
          resolve();
        });
      });
    },
    saveZone(sid, value) {
      let z = this.$firewall.findZoneByNetwork(this.editorIface);

      if (!value) {
        if (z)
          z.delNetwork(this.editorIface);
        return;
      }

      if (z) {
        if (value === z.name())
          return;
        z.delNetwork(this.editorIface);
      }

      z = this.$firewall.findZoneByName(value);
      if (!z)
        z = this.$firewall.createZone(value);
      z.addNetwork(this.editorIface);
    },
    edit(iface) {
      this.editorIface = iface;
      this.dialogVisible = true;
    },
    ifup(name) {
      this.$ubus.call('oui.network', 'ifup', {name: name});
    },
    ifdown(name) {
      this.$ubus.call('oui.network', 'ifdown', {name: name});
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
