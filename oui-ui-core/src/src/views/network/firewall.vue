<template>
  <div>
    <uci-form config="firewall" ref="form" tabbed>
      <uci-section type="defaults" :title="$t('General Settings')">
        <uci-option-switch :label="$t('Enable SYN-flood protection')" name="syn_flood"></uci-option-switch>
        <uci-option-switch :label="$t('Drop invalid packets')" name="drop_invalid"></uci-option-switch>
        <uci-option-list :label="$t('Input')" name="input" :options="targets" required></uci-option-list>
        <uci-option-list :label="$t('Output')" name="output" :options="targets" required></uci-option-list>
        <uci-option-list :label="$t('Forward')" name="forward" :options="targets" required></uci-option-list>
      </uci-section>
      <uci-section type="zone" :title="$t('Zones')" table addable :add="addZone" table-action-width="140">
        <uci-option-dummy :label="$t('Name')" name="name"></uci-option-dummy>
        <uci-option-list :label="$t('Input')" name="input" :options="targets" initial="ACCEPT" required></uci-option-list>
        <uci-option-list :label="$t('Output')" name="output" :options="targets" initial="ACCEPT" required></uci-option-list>
        <uci-option-list :label="$t('Forward')" name="forward" :options="targets" initial="REJECT" required></uci-option-list>
        <uci-option-switch :label="$t('Masquerading')" name="masq"></uci-option-switch>
        <uci-option-switch :label="$t('MSS clamping')" name="mtu_fix"></uci-option-switch>
        <template v-slot:action="{s, sid}">
          <el-button-group>
            <el-button type="primary" size="mini" @click="editZone(sid)">{{ $t('Edit') }}</el-button>
            <el-button type="danger" size="mini" @click="s.del(sid)">{{ $t('Delete') }}</el-button>
          </el-button-group>
        </template>
      </uci-section>
      <uci-section type="redirect" :title="$t('Port Forwards')" table addable :filter="filterRedirect" :add="addRedirect" table-action-width="140">
        <uci-option-input :label="$t('Name')" name="name"></uci-option-input>
        <uci-option-list :label="$t('Protocol')" name="proto" :options="protos" initial="tcp udp" allow-create></uci-option-list>
        <uci-option-list :label="$t('Source zone')" name="src" :options="zones" required></uci-option-list>
        <uci-option-input :label="$t('External port')" name="src_dport" rules="port"></uci-option-input>
        <uci-option-list :label="$t('Internal zone')" name="dest" :options="zones" required></uci-option-list>
        <uci-option-list :label="$t('Internal IP address')" name="dest_ip" :options="arps" allow-create rules="ip4addr"></uci-option-list>
        <uci-option-input :label="$t('Internal port')" name="dest_port" rules="port"></uci-option-input>
        <uci-option-switch :label="$t('Enable')" name="enabled" initial="1"></uci-option-switch>
        <template v-slot:action="{s, sid}">
          <el-button-group>
            <el-button type="primary" size="mini" @click="editDnat(sid)">{{ $t('Edit') }}</el-button>
            <el-button type="danger" size="mini" @click="s.del(sid)">{{ $t('Delete') }}</el-button>
          </el-button-group>
        </template>
      </uci-section>
      <uci-section type="rule" :title="$t('Traffic Rules')" table addable table-action-width="140" :add="addRule">
        <uci-option-input :label="$t('Name')" name="name"></uci-option-input>
        <uci-option-list :label="$t('Protocol')" name="proto" :options="protos" initial="tcp udp" allow-create></uci-option-list>
        <uci-option-list :label="$t('Source zone')" name="src" :options="zones"></uci-option-list>
        <uci-option-list :label="$t('Internal zone')" name="dest" :options="zones"></uci-option-list>
        <uci-option-input :label="$t('Destination port')" name="dest_port" :placeholder="$t('any')" rules="port" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
        <uci-option-list :label="$t('Action')" name="target" :options="actions" initial="DROP" required></uci-option-list>
        <template v-slot:action="{s, sid}">
          <el-button-group>
            <el-button type="primary" size="mini" @click="editRule(sid)">{{ $t('Edit') }}</el-button>
            <el-button type="danger" size="mini" @click="s.del(sid)">{{ $t('Delete') }}</el-button>
          </el-button-group>
        </template>
      </uci-section>
    </uci-form>
    <el-dialog :title="dialogZoneTitle" :visible.sync="dialogZoneVisible" custom-class="firewall-edit-dialog">
      <uci-form config="firewall" v-if="dialogZoneVisible" @apply="onApply">
        <uci-section :name="editorZone.sid">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option-list :label="$t('Input')" name="input" :options="targets" initial="ACCEPT" required></uci-option-list>
            <uci-option-list :label="$t('Output')" name="output" :options="targets" initial="ACCEPT" required></uci-option-list>
            <uci-option-list :label="$t('Forward')" name="forward" :options="targets" initial="REJECT" required></uci-option-list>
            <uci-option-switch :label="$t('Masquerading')" name="masq"></uci-option-switch>
            <uci-option-switch :label="$t('MSS clamping')" name="mtu_fix"></uci-option-switch>
            <uci-option-list :label="$t('Covered networks')" name="network" :options="interfaces" multiple></uci-option-list>
          </uci-tab>
          <uci-tab :title="$t('Advanced Settings')" name="advanced">
            <uci-option-list :label="$t('Restrict to address family')" name="family" :options="families"></uci-option-list>
            <uci-option-dlist :label="$t('Restrict Masquerading to given source subnets')" name="masq_src" :options="families" depend="family == '' || family == 'ipv4'"></uci-option-dlist>
            <uci-option-dlist :label="$t('Restrict Masquerading to given destination subnets')" name="masq_dest" :options="families" depend="family == '' || family == 'ipv4'"></uci-option-dlist>
            <uci-option-switch :label="$t('Force connection tracking')" name="conntrack"></uci-option-switch>
            <uci-option-switch :label="$t('Enable logging on this zone')" name="log"></uci-option-switch>
            <uci-option-input :label="$t('Limit log messages')" name="log_limit" placeholder="10/minute" depend="log"></uci-option-input>
          </uci-tab>
          <uci-tab :title="$t('Inter-Zone Forwarding')" name="fwd">
            <uci-option-list :label="$t('Allow forward to destination zones')" name="out" :load="loadDestZones" :save="saveDestZones" :options="zones.filter(name => name !== editorZone.name())" multiple></uci-option-list>
            <uci-option-list :label="$t('Allow forward from source zones')" name="in" :load="loadSrcZones" :save="saveSrcZones" :options="zones.filter(name => name !== editorZone.name())" multiple></uci-option-list>
          </uci-tab>
        </uci-section>
      </uci-form>
    </el-dialog>
    <el-dialog :title="dialogDnatTitle" :visible.sync="dialogDnatVisible" custom-class="firewall-edit-dialog">
      <uci-form config="firewall" v-if="dialogDnatVisible" @apply="onApply">
        <uci-section :name="editorDnatSid">
          <uci-option-input :label="$t('Name')" name="name"></uci-option-input>
          <uci-option-list :label="$t('Protocol')" name="proto" :options="protos" initial="tcp udp" allow-create></uci-option-list>
          <uci-option-list :label="$t('Source zone')" name="src" :options="zones" required></uci-option-list>
          <uci-option-dlist :label="$t('Source MAC address')" name="src_mac" rules="macaddr"></uci-option-dlist>
          <uci-option-input :label="$t('Source IP address')" name="src_ip" rules="ip4addr" :placeholder="$t('any')"></uci-option-input>
          <uci-option-input :label="$t('Source port')" name="src_port" rules="port" :placeholder="$t('any')" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
          <uci-option-input :label="$t('External IP address')" name="src_dip" rules="ip4addr" :placeholder="$t('any')"></uci-option-input>
          <uci-option-input :label="$t('External port')" name="src_dport" rules="port" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
          <uci-option-list :label="$t('Internal zone')" name="dest" :options="zones" required></uci-option-list>
          <uci-option-list :label="$t('Internal IP address')" name="dest_ip" :options="arps" allow-create rules="ip4addr"></uci-option-list>
          <uci-option-input :label="$t('Internal port')" name="dest_port" :placeholder="$t('any')" rules="port" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
          <uci-option-switch :label="$t('Enable NAT Loopback')" name="reflection" initial="1"></uci-option-switch>
        </uci-section>
      </uci-form>
    </el-dialog>
    <el-dialog :title="dialogRuleTitle" :visible.sync="dialogRuleVisible" custom-class="firewall-edit-dialog">
      <uci-form config="firewall" v-if="dialogRuleVisible" @apply="onApply">
        <uci-section :name="editorRuleSid">
          <uci-option-input :label="$t('Name')" name="name"></uci-option-input>
          <uci-option-list :label="$t('Protocol')" name="proto" :options="protos" initial="tcp udp" allow-create></uci-option-list>
          <uci-option-list :label="$t('Source zone')" name="src" :options="zones"></uci-option-list>
          <uci-option-list :label="$t('Internal zone')" name="dest" :options="zones"></uci-option-list>
          <uci-option-input :label="$t('Destination port')" name="dest_port" :placeholder="$t('any')" rules="port" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
          <uci-option-list :label="$t('Week Days')" name="weekdays" :options="weekdays" multiple></uci-option-list>
          <uci-option-list :label="$t('Month Days')" name="monthdays" :options="monthdays" multiple></uci-option-list>
          <uci-option-input :label="$t('Start Time (hh:mm:ss)')" name="start_time"></uci-option-input>
          <uci-option-input :label="$t('Stop Time (hh:mm:ss)')" name="stop_time"></uci-option-input>
          <uci-option-input :label="$t('Start Date (yyyy-mm-dd)')" name="start_date"></uci-option-input>
          <uci-option-input :label="$t('Stop Date (yyyy-mm-dd)')" name="stop_date"></uci-option-input>
          <uci-option-switch :label="$t('Time in UTC')" name="utc_time"></uci-option-switch>
          <uci-option-list :label="$t('Action')" name="target" :options="actions" initial="DROP" required></uci-option-list>
        </uci-section>
      </uci-form>
    </el-dialog>
  </div>
</template>

<script>
export default {
  data() {
    return {
      targets: [
        ['REJECT', this.$t('reject')],
        ['DROP', this.$t('drop')],
        ['ACCEPT', this.$t('accept')]
      ],
      dialogZoneVisible: false,
      editorZone: null,
      interfaces: [],
      families:[
        ['', this.$t('IPv4 and IPv6')],
        ['ipv4', this.$t('IPv4 only')],
        ['ipv6', this.$t('IPv6 only')]
      ],
      zones: [],
      protos: [
        ['tcp udp', 'TCP+UDP'],
        ['tcp', 'TCP'],
        ['udp', 'UDP'],
        ['icmp', 'ICMP']
      ],
      arps: [],
      editorDnatSid: '',
      dialogDnatVisible: false,
      editorRuleSid: '',
      dialogRuleVisible: false,
      actions: [
        ['DROP', this.$t('drop')],
        ['ACCEPT', this.$t('accept')],
        ['REJECT', this.$t('reject')],
        ['NOTRACK', this.$t('don\'t track')]
      ],
      weekdays: [
        ['Sun', this.$t('Sunday')],
        ['Mon', this.$t('Monday')],
        ['Tue', this.$t('Tuesday')],
        ['Wed', this.$t('Wednesday')],
        ['Thu', this.$t('Thursday')],
        ['Fri', this.$t('Friday')],
        ['Sat', this.$t('Saturday')]
      ],
      monthdays: []
    }
  },
  computed: {
    dialogZoneTitle() {
      if (!this.editorZone)
        return '';
      return `${this.$t('Zone')} "${this.editorZone.name()}"`
    },
    dialogDnatTitle() {
      if (!this.editorDnatSid)
        return '';
      const name = this.$uci.get('firewall', this.editorDnatSid, 'name') || '';
      return `${this.$t('Port Forwards')} "${name}"`
    },
    dialogRuleTitle() {
      if (!this.editorRuleSid)
        return '';
      const name = this.$uci.get('firewall', this.editorRuleSid, 'name') || '';
      return `${this.$t('Traffic Rules')} "${name}"`
    }
  },
  methods: {
    addZone(self) {
      this.$prompt(this.$t('Please input a name'), this.$t('Add'), {
        inputValidator: value => {
          if (!value)
            return true;

          const sections = self.sections;
          for (let i = 0; i < sections.length; i++)
            if (sections[i].name === value)
              return this.$t('Name already used');

          return true;
        }
      }).then(({value}) => {
        if (!value)
          return;

        const loading = this.$getLoading(this.$t('Loading...'));

        this.$firewall.createZone(value);

        this.$uci.save().then(() => {
          this.$uci.apply().then(() => {
            this.$uci.load('firewall').then(() => {
              this.$refs['form'].reset();
              this.load();
              loading.close();
            });
          });
        });
      });
    },
    editZone(sid) {
      this.editorZone = this.$firewall.findZoneBySid(sid);
      this.dialogZoneVisible = true;
    },
    editDnat(sid) {
      this.editorDnatSid = sid;
      this.dialogDnatVisible = true;
    },
    editRule(sid) {
      this.editorRuleSid = sid;
      this.dialogRuleVisible = true;
    },
    onApply() {
      this.$refs['form'].reset();
    },
    loadDestZones() {
      return this.editorZone.findForwardsBy('src').map(z => z.dest());
    },
    loadSrcZones() {
      return this.editorZone.findForwardsBy('dest').map(z => z.src());
    },
    saveDestZones(config, sid, name, value) {
      const dests = value;
      let i = 0;

      this.$firewall.forwards.forEach(fwd => {
        if (fwd.src() !== this.editorZone.name())
          return;

        if (i < dests.length)
          fwd.set('dest', dests[i++]);
        else
          this.$uci.del('firewall', fwd.sid);
      });

      while (i < dests.length) {
        const sid = this.$uci.add('firewall', 'forwarding');
        const fwd = new this.$firewall.Forward(sid);

        fwd.set('src', this.editorZone.name());
        fwd.set('dest', dests[i++]);
      }
    },
    saveSrcZones(config, sid, name, value) {
      const srcs = value;
      let i = 0;

      this.$firewall.forwards.forEach(fwd => {
        if (fwd.dest() !== this.editorZone.name())
          return;

        if (i < srcs.length)
          fwd.set('src', srcs[i++]);
        else
          this.$uci.del('firewall', fwd.sid);
      });

      while (i < srcs.length) {
        const sid = this.$uci.add('firewall', 'forwarding');
        const fwd = new this.$firewall.Forward(sid);

        fwd.set('src', srcs[i++]);
        fwd.set('dest', this.editorZone.name());
      }
    },
    filterRedirect(s) {
      return s.target !== 'SNAT';
    },
    addRedirect() {
      const loading = this.$getLoading(this.$t('Loading...'));

      const sid = this.$uci.add('firewall', 'redirect');

      this.$uci.set('firewall', sid, 'target', 'DNAT');
      this.$uci.set('firewall', sid, 'src', 'wan');
      this.$uci.set('firewall', sid, 'dest', 'lan');
      this.$uci.set('firewall', sid, 'proto', 'tcp udp');

      this.$uci.save().then(() => {
        this.$uci.apply().then(() => {
          this.$uci.load('firewall').then(() => {
            this.$refs['form'].reset();
            this.load();
            loading.close();
          });
        });
      });
    },
    addRule() {
      const loading = this.$getLoading(this.$t('Loading...'));

      const sid = this.$uci.add('firewall', 'rule');

      this.$uci.set('firewall', sid, 'target', 'ACCEPT');
      this.$uci.set('firewall', sid, 'src', 'wan');
      this.$uci.set('firewall', sid, 'proto', 'tcp udp');

      this.$uci.save().then(() => {
        this.$uci.apply().then(() => {
          this.$uci.load('firewall').then(() => {
            this.$refs['form'].reset();
            this.load();
            loading.close();
          });
        });
      });
    },
    getARPTable() {
      return this.$ubus.call('oui.network', 'arp_table');
    },
    load() {
      this.$network.load().then(() => {
        const interfaces = this.$network.getInterfaces();
        this.interfaces = interfaces.map(item => item.name);
      });

      this.$firewall.load().then(() => {
        this.zones = this.$firewall.zones.map(z => z.name());
      });
    }
  },
  created() {
    this.load();

    this.getARPTable().then(r => {
      r.entries.forEach(arp => {
        if (arp.macaddr === '00:00:00:00:00:00')
          return;

        this.arps.push([arp.ipaddr, `${arp.ipaddr} (${arp.macaddr})`]);
      });
    });

    for (let i = 1; i <= 31; i++)
      this.monthdays.push(i);
  }
}
</script>

<style lang="scss">
.firewall-edit-dialog {
  .el-dialog__header {
    padding: 10px 20px 10px;
  }
  .el-dialog__body {
    padding: 0;
  }
}
</style>
