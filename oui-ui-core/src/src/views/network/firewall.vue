<template>
  <div>
    <uci-form config="firewall" ref="form">
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
            <el-button type="primary" size="mini" @click="edit(sid)">{{ $t('Edit') }}</el-button>
            <el-button type="danger" size="mini" @click="s.del(sid)">{{ $t('Delete') }}</el-button>
          </el-button-group>
        </template>
      </uci-section>
    </uci-form>
    <el-dialog :title="dialogTitle" :visible.sync="dialogVisible" custom-class="zone-edit-dialog">
      <uci-form config="firewall" v-if="dialogVisible" @apply="onApply">
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
      dialogVisible: false,
      editorZone: null,
      interfaces: [],
      families:[
        ['', this.$t('IPv4 and IPv6')],
        ['ipv4', this.$t('IPv4 only')],
        ['ipv6', this.$t('IPv6 only')]
      ],
      zones: []
    }
  },
  computed: {
    dialogTitle() {
      if (!this.editorZone)
        return '';
      return `${this.$t('Zone')} "${this.editorZone.name()}"`
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
    edit(sid) {
      this.editorZone = this.$firewall.findZoneBySid(sid);
      this.dialogVisible = true;
    },
    onApply() {
      this.$refs['form'].reset();
    },
    loadDestZones(resolve) {
      const zones = this.editorZone.findForwardsBy('src').map(z => z.dest());
      resolve(zones);
    },
    loadSrcZones(resolve) {
      const zones = this.editorZone.findForwardsBy('dest').map(z => z.src());
      resolve(zones);
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
  }
}
</script>

<style lang="scss">
.zone-edit-dialog {
  .el-dialog__header {
    padding: 10px 20px 10px;
  }
  .el-dialog__body {
    padding: 0;
  }
}
</style>
