<template>
  <div :style="{margin: 'auto 10px auto', backgroundColor: color, height: width + 'px', width: length}" style="text-align: center;">
    <img v-if="disconnect" src="/icons/error.png" :style="{marginTop: (width / 2 - 20) + 'px'}" />
  </div>
</template>

<script>
export default {
  name: 'OuiLine',
  props: {
    color: {
      type: String,
      default: '#56E67E'
    },
    width: {
      type: Number,
      default: 8
    },
    length: {
      type: String,
      default: '100%'
    },
    disconnect: Boolean
  }
}
</script>
