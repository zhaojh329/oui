import{_ as n,o as s,c as a,e}from"./app.9ff75c61.js";const t={},i=e(`<h1 id="lua-api" tabindex="-1"><a class="header-anchor" href="#lua-api" aria-hidden="true">#</a> Lua API</h1><p>In Oui, Lua API are organized as <code>module-methods</code>.</p><div class="language-bash ext-sh line-numbers-mode"><pre class="language-bash"><code>root@OpenWrt:~<span class="token comment"># ls /usr/share/oui/rpc/</span>
acl.lua       network.lua   ubus.lua      ui.lua        wireless.lua
demo.lua      system.lua    uci.lua       user.lua
</code></pre><div class="line-numbers" aria-hidden="true"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><p>Each Lua file here represents a module. Module name is Lua file name(without suffix).</p><p>Each Lua API file needs to return a <code>Lua Table</code>, which consists of multiple <code>Lua functions</code>.</p><div class="language-lua ext-lua line-numbers-mode"><pre class="language-lua"><code><span class="token comment">-- /usr/share/oui/rpc/test.lua</span>

<span class="token keyword">local</span> M <span class="token operator">=</span> <span class="token punctuation">{</span><span class="token punctuation">}</span>

<span class="token comment">--[[
params: Parameters passed by the front-end call
section: The login session information is a Table.
         Contains the currently logged in username (username) and the permission group (acl) to which it belongs.
--]]</span>
<span class="token keyword">function</span> M<span class="token punctuation">.</span><span class="token function">func1</span><span class="token punctuation">(</span>params<span class="token punctuation">,</span> section<span class="token punctuation">)</span>
    <span class="token keyword">local</span> res <span class="token operator">=</span> <span class="token punctuation">{</span><span class="token punctuation">}</span>
    <span class="token punctuation">...</span>
    <span class="token keyword">return</span> res
<span class="token keyword">end</span>

<span class="token keyword">return</span> M
</code></pre><div class="line-numbers" aria-hidden="true"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><div class="language-javascript ext-js line-numbers-mode"><pre class="language-javascript"><code><span class="token keyword">this</span><span class="token punctuation">.</span><span class="token function">$oui</span><span class="token punctuation">.</span><span class="token function">call</span><span class="token punctuation">(</span><span class="token string">&#39;test&#39;</span><span class="token punctuation">,</span> <span class="token string">&#39;func1&#39;</span><span class="token punctuation">,</span> <span class="token punctuation">{</span><span class="token literal-property property">a</span><span class="token operator">:</span> <span class="token number">1</span><span class="token punctuation">}</span><span class="token punctuation">)</span><span class="token punctuation">.</span><span class="token function">then</span><span class="token punctuation">(</span><span class="token parameter">res</span> <span class="token operator">=&gt;</span> <span class="token punctuation">{</span>
    <span class="token operator">...</span>
<span class="token punctuation">}</span><span class="token punctuation">)</span>
</code></pre><div class="line-numbers" aria-hidden="true"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div>`,7),l=[i];function c(o,p){return s(),a("div",null,l)}const r=n(t,[["render",c],["__file","lua-api.html.vue"]]);export{r as default};
