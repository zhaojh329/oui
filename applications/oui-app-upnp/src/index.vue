<template>
  <el-tabs value="status">
    <el-tab-pane name="status" :label="$t('Active UPnP Redirects')">
      <el-table :data="activeRedirects">
        <el-table-column prop="proto" :label="$t('Protocol')"></el-table-column>
        <el-table-column prop="extport" :label="$t('External port')"></el-table-column>
        <el-table-column prop="intaddr" :label="$t('Client Address')"></el-table-column>
        <el-table-column prop="host_hint" :label="$t('Host')"></el-table-column>
        <el-table-column prop="intport" :label="$t('Client Port')"></el-table-column>
        <el-table-column prop="descr" :label="$t('Description')"></el-table-column>
      </el-table>
    </el-tab-pane>
    <el-tab-pane name="setting" :label="$t('MiniUPnP settings')">
      <uci-form config="upnpd">
        <uci-section name="config">
          <uci-tab name="general" :title="$t('General Settings')">
            <uci-option-switch name="enabled" :label="$t('Start UPnP and NAT-PMP service')" initial="1" @applied="applyEnable"></uci-option-switch>
            <uci-option-switch name="enable_upnp" :label="$t('Enable UPnP functionality')" initial="1"></uci-option-switch>
            <uci-option-switch name="enable_natpmp" :label="$t('Enable NAT-PMP functionality')" initial="1"></uci-option-switch>
            <uci-option-switch name="secure_mode" :label="$t('Enable secure mode')" initial="1" :description="$t('Allow adding forwards only to requesting ip addresses')"></uci-option-switch>
            <uci-option-switch name="igdv1" :label="$t('Enable IGDv1 mode')" :description="$t('Advertise as IGDv1 device instead of IGDv2')"></uci-option-switch>
            <uci-option-switch name="log_output" :label="$t('Enable additional logging')" :description="$t('Puts extra debugging information into the system log')"></uci-option-switch>
            <uci-option-input name="download" :label="$t('Downlink')" :description="$t('Value in KByte/s, informational only')"></uci-option-input>
            <uci-option-input name="upload" :label="$t('Uplink')" :description="$t('Value in KByte/s, informational only')"></uci-option-input>
            <uci-option-input name="port" :label="$t('Port')" initial="5000" rules="port"></uci-option-input>
          </uci-tab>
          <uci-tab name="advanced" :title="$t('Advanced Settings')">
            <uci-option-switch name="system_uptime" :label="$t('Report system instead of daemon uptime')" initial="1"></uci-option-switch>
            <uci-option-input name="uuid" :label="$t('Device UUID')"></uci-option-input>
            <uci-option-input name="serial_number" :label="$t('Announced serial number')"></uci-option-input>
            <uci-option-input name="model_number" :label="$t('Announced model number')"></uci-option-input>
            <uci-option-input name="notify_interval" :label="$t('Notify interval')" placeholder="30" rules="uinteger"></uci-option-input>
            <uci-option-input name="clean_ruleset_threshold" :label="$t('Clean rules threshold')" placeholder="20" rules="uinteger"></uci-option-input>
            <uci-option-input name="clean_ruleset_interval" :label="$t('Clean rules interval')" placeholder="600" rules="uinteger"></uci-option-input>
            <uci-option-input name="presentation_url" :label="$t('Presentation URL')" placeholder="http://192.168.1.1/"></uci-option-input>
            <uci-option-file name="upnp_lease_file" :label="$t('UPnP lease file')" placeholder="/var/run/miniupnpd.leases"></uci-option-file>
          </uci-tab>
        </uci-section>
        <uci-section type="perm_rule" title="MiniUPnP ACLs" table sortable addable>
          <uci-option-input name="comment" :label="$t('Comment')"></uci-option-input>
          <uci-option-input name="ext_ports" :label="$t('External ports')" placeholder="0-65535"></uci-option-input>
          <uci-option-input name="int_addr" :label="$t('Internal addresses')" rules="ip4addr" placeholder="0.0.0.0/0"></uci-option-input>
          <uci-option-input name="int_ports" :label="$t('Internal ports')" placeholder="0-65535"></uci-option-input>
          <uci-option-list name="action" :label="$t('Action')" :options="actions"></uci-option-list>
        </uci-section>
      </uci-form>
    </el-tab-pane>
  </el-tabs>
</template>

<script>
  export default {
    data() {
      return {
        activeRedirects: [],
        actions: [
          ['allow', this.$t('Allow')],
          ['deny', this.$t('Deny')]
        ]
      }
    },
    methods: {
      applyEnable(value) {
        if (value) {
          this.$system.initStart('miniupnpd');
        } else {
          this.$system.initStop('miniupnpd');
        }
      }
    },
    created() {
      this.$ubus.call('upnp', 'status').then(r => {
        this.activeRedirects = r.entries;
      });
    }
  }
</script>
