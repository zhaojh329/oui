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
        <uci-section :name="editorZoneSid">
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
      editorZoneSid: '',
      editorZoneName: '',
      interfaces: [],
      families:[
        ['', this.$t('IPv4 and IPv6')],
        ['ipv4', this.$t('IPv4 only')],
        ['ipv6', this.$t('IPv6 only')]
      ]
    }
  },
  computed: {
    dialogTitle() {
      return `${this.$t('Zone')} "${this.editorZoneName}"`
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

        const sid = this.$uci.add('firewall', 'zone');
        this.$uci.set('firewall', sid, 'name', value);
        self.postAdd(sid);
      });
    },
    edit(sid) {
      this.editorZoneSid = sid;
      this.dialogVisible = true;
      this.editorZoneName = this.$uci.get('firewall', sid, 'name');
    },
    onApply() {
      this.$refs['form'].reset();
    }
  },
  created() {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces = interfaces.map(item => item.name);
    });
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
