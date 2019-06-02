<template>
  <el-badge v-if="value > 0" :value="value"></el-badge>
</template>

<script>
export default {
  name: 'UciBadge',
  props: {
    value: Number
  }
}
</script>

