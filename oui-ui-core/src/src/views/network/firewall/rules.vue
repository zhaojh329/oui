<template>
  <uci-section type="rule" :title="$t('Traffic Rules')" table addable sortable>
    <uci-option-input :label="$t('Name')" name="name"></uci-option-input>
    <uci-option-list :label="$t('Protocol')" name="proto" :options="protos" initial="tcp udp" allow-create></uci-option-list>
    <uci-option-list :label="$t('Source zone')" name="src" :options="zones"></uci-option-list>
    <uci-option-list :label="$t('Internal zone')" name="dest" :options="zones"></uci-option-list>
    <uci-option-input :label="$t('Destination port')" name="dest_port" :placeholder="$t('any')" rules="port" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
    <uci-option-list :label="$t('Action')" name="target" :options="actions" initial="DROP" required></uci-option-list>
    <uci-table-expand>
      <uci-option-list :label="$t('Week Days')" name="weekdays" :options="weekdays" multiple></uci-option-list>
      <uci-option-list :label="$t('Month Days')" name="monthdays" :options="monthdays" multiple></uci-option-list>
      <uci-option-input :label="$t('Start Time (hh:mm:ss)')" name="start_time"></uci-option-input>
      <uci-option-input :label="$t('Stop Time (hh:mm:ss)')" name="stop_time"></uci-option-input>
      <uci-option-input :label="$t('Start Date (yyyy-mm-dd)')" name="start_date"></uci-option-input>
      <uci-option-input :label="$t('Stop Date (yyyy-mm-dd)')" name="stop_date"></uci-option-input>
      <uci-option-switch :label="$t('Time in UTC')" name="utc_time"></uci-option-switch>
    </uci-table-expand>
  </uci-section>
</template>

<script>
export default {
  props: {
    zones: Array,
    protos: Array
  },
  data() {
    return {
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
      monthdays: [...Array(31).keys()].map(i => i + 1)
    }
  }
}
</script>
