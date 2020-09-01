<template>
  <div class="oui-dashboard">
    <a-progress :percent="percent" :stroke-color="color" type="dashboard">
      <template #format>
        <h2 :style="{color: color}">{{ percent }}%</h2>
        <h4>{{ label }}</h4>
      </template>
    </a-progress>
  </div>
</template>

<script>
export default {
  name: 'OuiDashboard',
  props: {
    percent: Number,
    label: String
  },
  computed: {
    color () {
      if (this.percent < 60) { return '#2d8cf0' }
      if (this.percent < 80) { return '#ff9900' }
      return '#ed4014'
    }
  }
}
</script>

<style lang="stylus">
  .oui-dashboard {
    background-color: white;
    border-radius: 50%;
    padding: 5px;
  }
</style>
