#
# hmi_generator.rb
#
# Copyright (c) 2012 project bchan
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source
#    distribution.
#

require 'yaml'
require 'erb'
require 'jcode'

def conv_euc_to_TCArray(str)
  ret = Array.new();
  str.each_char do |x|
    i = x.length - 1;
    val = 0;
    x.each_byte do |ch|
      val += ch << (8 * i);
      i -= 1;
    end
    ret.push(val & 0x7f7f);
  end 
  ret;
end

def conv_TCArray_to_hex_definition(a)
  str = String.new();
  a.each do |x|
    str += "0x" + x.to_s(16) + ", "
  end
  str += "TNULL";
end

def calc_euc_to_TCArray_length(str)
  str = conv_euc_to_TCArray(str);
  return str.length;
end

class HMIParts
  attr_accessor :yaml
  def initialize(yaml)
    @yaml = yaml;
  end
  def type()
    @yaml["type"];
  end
  def name()
    @yaml["name"];
  end
  def text_length()
    @yaml["length"];
  end
  def rect_string()
    "{{" + @yaml["rect"]["left"].to_s + ", " + @yaml["rect"]["top"].to_s + ", " + @yaml["rect"]["right"].to_s + ", " + @yaml["rect"]["bottom"].to_s + "}}";
  end
  def rect_left()
    @yaml["rect"]["left"].to_s
  end
  def rect_top()
    @yaml["rect"]["top"].to_s
  end
  def rect_right()
    @yaml["rect"]["right"].to_s
  end
  def rect_bottom()
    @yaml["rect"]["bottom"].to_s
  end
  def text_array()
    conv_TCArray_to_hex_definition(conv_euc_to_TCArray(@yaml["text"]));
  end
  def text_array_length()
    calc_euc_to_TCArray_length(@yaml["text"]);
  end
  def is_attr_specified()
    @yaml["attr"] != nil
  end
  def get_attr_offset()
    if self.is_attr_specified()
      7
    else
      1
    end
  end
  def get_attrval_chcol()
    if @yaml["attr"]["chcol"] == nil
      return -1
    end
    @yaml["attr"]["chcol"]
  end
  def get_attrval_class()
    if @yaml["attr"]["class"] == nil
      return -1
    end
    @yaml["attr"]["class"]
  end
  def get_attrval_attr()
    if @yaml["attr"]["attr"] == nil
      return -1
    end
    @yaml["attr"]["attr"]
  end
  def get_attrval_csize_h()
    if @yaml["attr"]["csize"] == nil || @yaml["attr"]["csize"]["h"] == nil
      return 16
    end
    @yaml["attr"]["csize"]["h"]
  end
  def get_attrval_csize_v()
    if @yaml["attr"]["csize"] == nil || @yaml["attr"]["csize"]["v"] == nil
      return 16
    end
    @yaml["attr"]["csize"]["v"]
  end
  def is_need_eventbreak()
    return false
  end
  def get_databox_specify()
    return @yaml["databox"]["specify"]
  end
  def get_databox_number()
    return @yaml["databox"]["number"]
  end
  def is_databox_specify_argument()
    @yaml["databox"] != nil && @yaml["databox"]["specify"] == "argument"
  end
  def is_databox_use()
    @yaml["databox"] != nil && (@yaml["databox"]["specify"] != "direct" || @yaml["databox"]["specify"] != "argument")
  end
  def is_use_rect_in_open()
    !is_databox_use()
  end

  def generate_header_eventtype_enumulate(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventstruct_definition(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventunion_member(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_prototypes(window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct(window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_functions(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_in_new()
    script = <<-EOS
	window-><%= self.name() %>.id = -1;
	<%- if self.is_databox_specify_argument() -%>
	window-><%= self.name() %>.dnum = dnum_<%= self.name() %>;
	<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_databox()
    case self.get_databox_specify()
    when "argument"
      script = "copn_par(wid, window-><%= self.name() %>.dnum, NULL)"
    when "specify"
      script = "copn_par(wid, <%= self.get_databox_number() %>, NULL)"
    end

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_direct()
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall()
    if self.is_databox_use()
      return self.generate_create_systemcall_databox()
    else
      return self.generate_create_systemcall_direct()
    end
  end

  def generate_create_in_open()
    script = <<-EOS
	<%- if !self.is_databox_use() -%>
	r = (RECT)<%= self.rect_string() %>;
	<%- end -%>
	window-><%= self.name() %>.id = <%= self.generate_create_systemcall()%>;
	if (window-><%= self.name() %>.id < 0) {
		DP_ER("ccre_xxx <%= self.name() %> error:", window-><%= self.name() %>.id);
	}
	<%- if self.is_databox_use() -%><%= self.generate_loadvalue_in_open() %><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_loadvalue_in_open()
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_savevalue_in_close()
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_action_in_butdnwork(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_draw_in_draw(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMITextBoxParts < HMIParts
  def is_need_eventbreak()
    return true
  end

  def generate_header_eventtype_enumulate(main_name, window_name)
    script = <<-EOS
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_DETERMINE,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_COPY,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MOVE,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MENU,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_KEYMENU,
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventstruct_definition(main_name, window_name)
    script = <<-EOS
struct <%= window_name %>_eventdata_<%= self.name() %>_determine_t_ {
	TC *value;
	W len;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_determine_t_ <%= window_name %>_eventdata_<%= self.name() %>_determine_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_copy_t_ {
	WID rel_wid;
	PNT pos;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_copy_t_ <%= window_name %>_eventdata_<%= self.name() %>_copy_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_move_t_ {
	WID rel_wid;
	PNT pos;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_move_t_ <%= window_name %>_eventdata_<%= self.name() %>_move_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_menu_t_ {
	PNT pos;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_menu_t_ <%= window_name %>_eventdata_<%= self.name() %>_menu_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_keymenu_t_ {
	TC keycode;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_keymenu_t_ <%= window_name %>_eventdata_<%= self.name() %>_keymenu_t;

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventunion_member(main_name, window_name)
    script = <<-EOS
		<%= window_name %>_eventdata_<%= self.name() %>_determine_t <%= window_name %>_<%= self.name() %>_determine;
		<%= window_name %>_eventdata_<%= self.name() %>_copy_t <%= window_name %>_<%= self.name() %>_copy;
		<%= window_name %>_eventdata_<%= self.name() %>_move_t <%= window_name %>_<%= self.name() %>_move;
		<%= window_name %>_eventdata_<%= self.name() %>_menu_t <%= window_name %>_<%= self.name() %>_menu;
		<%= window_name %>_eventdata_<%= self.name() %>_keymenu_t <%= window_name %>_<%= self.name() %>_keymenu;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_prototypes(window_name)
    script = <<-EOS
IMPORT W <%= window_name %>_set<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len);
IMPORT W <%= window_name %>_get<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len);
IMPORT W <%= window_name %>_cut<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len, Bool cut);
IMPORT W <%= window_name %>_insert<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len);
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct(window_name)
    script = <<-EOS
	struct {
		PAID id;
		<% if self.is_databox_specify_argument() %>W dnum;<% end %>
		TC buf[<%= self.text_length() %>+<%= self.get_attr_offset() %>+1];
		W buf_written;
		Bool appended;
		Bool nextaction;
	} <%= self.name() %>;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_functions(main_name, window_name)
    script = <<-EOS
EXPORT W <%= window_name %>_set<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len)
{
	W cp_len;

	if (len < <%= self.text_length() %>) {
		cp_len = len;
	} else {
		cp_len = <%= self.text_length() %>;
	}
	memcpy(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>, str, cp_len * sizeof(TC));
	window-><%= self.name() %>.buf[<%= self.get_attr_offset() %> + cp_len] = TNULL;
	window-><%= self.name() %>.buf_written = cp_len;
	if (window->wid < 0) {
		return 0;
	}

	return cset_val(window-><%= self.name() %>.id, cp_len, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
}

EXPORT W <%= window_name %>_get<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len)
{
	W err, cp_len;

	if (window->wid > 0) {
		err = cget_val(window-><%= self.name() %>.id, <%= self.text_length() %>, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
		if (err < 0) {
			return err;
		}
		window-><%= self.name() %>.buf_written = err;
	}

	if (len < window-><%= self.name() %>.buf_written) {
		cp_len = len;
	} else {
		cp_len = window-><%= self.name() %>.buf_written;
	}
	memcpy(str, window-><%= self.name() %>.buf + <%= self.get_attr_offset() %>, cp_len * sizeof(TC));

	return cp_len;
}

EXPORT W <%= window_name %>_cut<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len, Bool cut)
{
	W err, len0;

	len0 = ccut_txt(window-><%= self.name() %>.id, len, str, cut == False ? 0 : 1);
	if (len0 < 0) {
		return len0;
	}

	if (cut != False) {
		err = cget_val(window-><%= self.name() %>.id, <%= self.text_length() %>, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
		if (err < 0) {
			return err;
		}
		window-><%= self.name() %>.buf_written = err;
		return len0;
	}

	return len0;
}

EXPORT W <%= window_name %>_insert<%= self.name() %>text(<%= window_name %>_t *window, TC *str, W len)
{
	W err;

	err = cins_txt(window-><%= self.name() %>.id, (PNT){0x8000, 0x8000}, str);
	if (err < 0) {
		return err;
	}

	err = cget_val(window-><%= self.name() %>.id, <%= self.text_length() %>, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
	if (err < 0) {
		return err;
	}
	window-><%= self.name() %>.buf_written = err;

	return err;
}

LOCAL VOID <%= window_name %>_action<%= self.name() %>(<%= window_name %>_t *window, WEVENT *wev, <%= main_name %>event_t *evt)
{
	W i, len;

	i = cact_par(window-><%= self.name() %>.id, wev);
	if (i & 0x2000) {
		window-><%= self.name() %>.nextaction = True;
		switch (i) {
		case	P_MENU:
			if ((wev->s.type == EV_KEYDWN)&&(wev->s.stat & ES_CMD)) {
				evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_KEYMENU;
				evt->data.<%= window_name %>_<%= self.name() %>_keymenu.keycode = wev->e.data.key.code;
			} else {
				evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MENU;
				evt->data.<%= window_name %>_<%= self.name() %>_menu.pos = wev->s.pos;
			}
			<%= window_name %>_setflag(window, <%= window_name.upcase %>_FLAG_PARTS_NEXTACTION);
			break;
		default:
			wugt_evt(wev);
			<%= window_name %>_setflag(window, <%= window_name.upcase %>_FLAG_PARTS_OTHEREVENT);
			break;
		}
		return;
	}
	window-><%= self.name() %>.nextaction = False;
	if (i & 0x1000) {
		len = cget_val(window-><%= self.name() %>.id, <%= self.text_length() %>, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
		if (len > 0) {
			window-><%= self.name() %>.buf_written = len;
		}
	}
	switch (i & 7) {
	case	P_BUT:
		cchg_par(window-><%= self.name() %>.id, P_INACT);
		cchg_par(window-><%= self.name() %>.id, P_ACT);
		wugt_evt(wev);
		break;
	case	P_TAB:
		break;
	case	P_NL:
	case	P_END:
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_DETERMINE;
		evt->data.<%= window_name %>_<%= self.name() %>_determine.value = window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>;
		evt->data.<%= window_name %>_<%= self.name() %>_determine.len = window-><%= self.name() %>.buf_written;
		break;
	case	P_MOVE:
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MOVE;
		evt->data.<%= window_name %>_<%= self.name() %>_move.rel_wid = wev->s.wid;
		evt->data.<%= window_name %>_<%= self.name() %>_move.pos = wev->s.pos;
		break;
	case	P_COPY:
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_COPY;
		evt->data.<%= window_name %>_<%= self.name() %>_copy.rel_wid = wev->s.wid;
		evt->data.<%= window_name %>_<%= self.name() %>_copy.pos = wev->s.pos;
		break;
	}
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_in_new()
    script = <<-EOS
	window-><%= self.name() %>.id = -1;
	<%- if self.is_databox_specify_argument() -%>
	window-><%= self.name() %>.dnum = dnum_<%= self.name() %>;
	<%- end -%>
	memset(window-><%= self.name() %>.buf, 0, sizeof(TC)*<%= self.text_length() %>);
	<%- if self.is_attr_specified() -%>
	window-><%= self.name() %>.buf[0] = MC_ATTR;
	*(COLOR*)(window-><%= self.name() %>.buf + 1) = <%= self.get_attrval_chcol() %>;
	window-><%= self.name() %>.buf[3] = <%= self.get_attrval_class() %>;
	window-><%= self.name() %>.buf[4] = <%= self.get_attrval_attr() %>;
	window-><%= self.name() %>.buf[5] = <%= self.get_attrval_csize_h() %>;
	window-><%= self.name() %>.buf[6] = <%= self.get_attrval_csize_v() %>;
	window-><%= self.name() %>.buf[7] = TNULL;
	<%- else -%>
	window-><%= self.name() %>.buf[0] = MC_STR;
	window-><%= self.name() %>.buf[1] = TNULL;
	<%- end -%>
	window-><%= self.name() %>.nextaction = False;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_direct()
    script = "ccre_tbx(wid, TB_PARTS|P_DISP, &r, <%= self.text_length() %>, window-><%= self.name() %>.buf, NULL)"

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_loadvalue_in_open()
    script = <<-EOS
	cset_val(window-><%= self.name() %>.id, <%= self.text_length() %>, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_savevalue_in_close()
    script = <<-EOS
	err = cget_val(window-><%= self.name() %>.id, <%= self.text_length() %>, (W*)(window-><%= self.name() %>.buf+<%= self.get_attr_offset() %>));
	if (err >= 0) {
		window-><%= self.name() %>.buf_written = err;
	}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_action_in_butdnwork(main_name, window_name)
    script = <<-EOS
	if (id == window-><%= self.name() %>.id) {
		memcpy(&window->savedwev, wev, sizeof(WEVENT));
		<%= window_name %>_action<%= self.name() %>(window, wev, evt);
		return;
	}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMISecretTextBoxParts < HMITextBoxParts
  def generate_create_systemcall_direct()
    script = "ccre_xbx(wid, XB_PARTS|P_DISP, &r, <%= self.text_length() %>, window-><%= self.name() %>.buf, NULL);";

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMINumberBoxParts < HMIParts
  def is_need_eventbreak()
    return true
  end
  def is_double()
    if @yaml["format"]["datatype"] == "double"
      return true
    end
    return false
  end
  def is_pointdisplay()
    ret = @yaml["format"]["pointdisplay"];
    if ret == nil
      return false
    end
    return ret
  end
  def is_hankaku()
    ret = @yaml["format"]["hankaku"];
    if ret == nil
      return false
    end
    return ret
  end
  def get_fontsize()
    sz = @yaml["format"]["fontsize"]
    if sz == nil
      return 16
    end
    if sz < 0
      return 16
    end
    if sz > 255
      return 255
    end
    return sz
  end
  def get_decimalplace()
    sz = @yaml["format"]["decimalplace"]
    if sz == nil
      return 0
    end
    if sz < 0
      return 0
    end
    if sz > 15
      return 15
    end
    return sz
  end

  def generate_header_eventtype_enumulate(main_name, window_name)
    script = <<-EOS
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_DETERMINE,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_COPY,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MOVE,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MENU,
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_KEYMENU,
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventstruct_definition(main_name, window_name)
    script = <<-EOS
struct <%= window_name %>_eventdata_<%= self.name() %>_determine_t_ {
	<% if self.is_double() %>DOUBLE<% else %>W<% end %> value;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_determine_t_ <%= window_name %>_eventdata_<%= self.name() %>_determine_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_copy_t_ {
	WID rel_wid;
	PNT pos;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_copy_t_ <%= window_name %>_eventdata_<%= self.name() %>_copy_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_move_t_ {
	WID rel_wid;
	PNT pos;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_move_t_ <%= window_name %>_eventdata_<%= self.name() %>_move_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_menu_t_ {
	PNT pos;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_menu_t_ <%= window_name %>_eventdata_<%= self.name() %>_menu_t;

struct <%= window_name %>_eventdata_<%= self.name() %>_keymenu_t_ {
	TC keycode;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_keymenu_t_ <%= window_name %>_eventdata_<%= self.name() %>_keymenu_t;

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventunion_member(main_name, window_name)
    script = <<-EOS
		<%= window_name %>_eventdata_<%= self.name() %>_determine_t <%= window_name %>_<%= self.name() %>_determine;
		<%= window_name %>_eventdata_<%= self.name() %>_copy_t <%= window_name %>_<%= self.name() %>_copy;
		<%= window_name %>_eventdata_<%= self.name() %>_move_t <%= window_name %>_<%= self.name() %>_move;
		<%= window_name %>_eventdata_<%= self.name() %>_menu_t <%= window_name %>_<%= self.name() %>_menu;
		<%= window_name %>_eventdata_<%= self.name() %>_keymenu_t <%= window_name %>_<%= self.name() %>_keymenu;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_prototypes(window_name)
    script = <<-EOS
IMPORT W <%= window_name %>_set<%= self.name() %>value(<%= window_name %>_t *window, <% if self.is_double() %>DOUBLE<% else %>W<% end %> value);
IMPORT W <%= window_name %>_get<%= self.name() %>value(<%= window_name %>_t *window, <% if self.is_double() %>DOUBLE<% else %>W<% end %> *value);
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct(window_name)
    script = <<-EOS
	struct {
		PAID id;
		<% if self.is_databox_specify_argument() %>W dnum;<% end %>
		UW format;
		<% if self.is_double() %>DOUBLE<% else %>W<% end %> cv;
		Bool nextaction;
	} <%= self.name() %>;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_functions(main_name, window_name)
    script = <<-EOS
EXPORT W <%= window_name %>_set<%= self.name() %>value(<%= window_name %>_t *window, <% if self.is_double() %>DOUBLE<% else %>W<% end %> value)
{
	window-><%= self.name() %>.cv = value;
	if (window->wid < 0) {
		return 0;
	}

	return cset_val(window-><%= self.name() %>.id, <% if self.is_double() %>2<% else %>1<% end %>, (W*)(&window-><%= self.name() %>.cv));
}

EXPORT W <%= window_name %>_get<%= self.name() %>value(<%= window_name %>_t *window, <% if self.is_double() %>DOUBLE<% else %>W<% end %> *value)
{
	W err;

	if (window->wid > 0) {
		err = cget_val(window-><%= self.name() %>.id, <% if self.is_double() %>2<% else %>1<% end %>, (W*)(&window-><%= self.name() %>.cv));
		if (err < 0) {
			return err;
		}
	}

	*value = window-><%= self.name() %>.cv;
	return 0;
}

LOCAL VOID <%= window_name %>_action<%= self.name() %>(<%= window_name %>_t *window, WEVENT *wev, <%= main_name %>event_t *evt)
{
	W i;

	i = cact_par(window-><%= self.name() %>.id, wev);
	if (i & 0x2000) {
		window-><%= self.name() %>.nextaction = True;
		<%= window_name %>_setflag(window, <%= window_name.upcase %>_FLAG_PARTS_OTHEREVENT);
		wugt_evt(wev);
		return;
	}
	window-><%= self.name() %>.nextaction = False;
	if (i & 0x1000) {
		cget_val(window-><%= self.name() %>.id, <% if self.is_double() %>2<% else %>1<% end %>, (W*)(&window-><%= self.name() %>.cv));
	}
	switch (i & 7) {
	case	P_BUT:
		wugt_evt(wev);
		break;
	case	P_TAB:
		break;
	case	P_NL:
	case	P_END:
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_DETERMINE;
		evt->data.<%= window_name %>_<%= self.name() %>_determine.value = window-><%= self.name() %>.cv;
		break;
	case	P_MOVE:
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MOVE;
		evt->data.<%= window_name %>_<%= self.name() %>_move.rel_wid = wev->s.wid;
		evt->data.<%= window_name %>_<%= self.name() %>_move.pos = wev->s.pos;
		break;
	case	P_COPY:
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_COPY;
		evt->data.<%= window_name %>_<%= self.name() %>_copy.rel_wid = wev->s.wid;
		evt->data.<%= window_name %>_<%= self.name() %>_copy.pos = wev->s.pos;
		break;
	case	P_MENU:
		if ((wev->s.type == EV_KEYDWN)&&(wev->s.stat & ES_CMD)) {
			evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_KEYMENU;
			evt->data.<%= window_name %>_<%= self.name() %>_keymenu.keycode = wev->e.data.key.code;
		} else {
			evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_MENU;
			evt->data.<%= window_name %>_<%= self.name() %>_menu.pos = wev->s.pos;
		}
		window-><%= self.name() %>.nextaction = True;
		<%= window_name %>_setflag(window, <%= window_name.upcase %>_FLAG_PARTS_NEXTACTION);
		wugt_evt(wev);
		break;
	}
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_in_new()
    script = <<-EOS
	window-><%= self.name() %>.id = -1;
	<%- if self.is_databox_specify_argument() -%>
	window-><%= self.name() %>.dnum = dnum_<%= self.name() %>;
	<%- end -%>
	window-><%= self.name() %>.format = (<%= self.get_fontsize() %> << 8)|<% if self.is_double() %>0xF0<% else %>0<% end %>|<% if self.is_hankaku() %>0x40<% else %>0<% end %>|<% if self.is_pointdisplay() %>0x10<% else %>0<% end %>|<%=  self.get_decimalplace() %>;
	window-><%= self.name() %>.cv = <% if self.is_double() %>0.0<% else %>0<% end %>;
	window-><%= self.name() %>.nextaction = False;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_direct()
    script = "ccre_nbx(wid, NB_PARTS|P_DISP, &r, window-><%= self.name() %>.format, (W*)&window-><%= self.name() %>.cv, NULL)"

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_loadvalue_in_open()
    script = <<-EOS
	cset_val(window-><%= self.name() %>.id, <% if self.is_double() %>2<% else %>1<% end %>, (W*)(&window-><%= self.name() %>.cv));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_savevalue_in_close()
    script = <<-EOS
	err = cget_val(window-><%= self.name() %>.id, <% if self.is_double() %>2<% else %>1<% end %>, (W*)(&window-><%= self.name() %>.cv));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_action_in_butdnwork(main_name, window_name)
    script = <<-EOS
	if (id == window-><%= self.name() %>.id) {
		<%= window_name %>_action<%= self.name() %>(window, wev, evt);
		return;
	}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMITextMomentallySwitchParts < HMIParts
  def generate_header_eventtype_enumulate(main_name, window_name)
    script = <<-EOS
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_PUSH,
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct(window_name)
    script = <<-EOS
	struct {
		PAID id;
		<% if self.is_databox_specify_argument() %>W dnum;<% end %>
	} <%= self.name() %>;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_direct()
    script = "ccre_msw(wid, MS_PARTS|P_DISP, &r, (TC[]){MC_STR, <%= self.text_array() %>}, NULL)"

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_action_in_butdnwork(main_name, window_name)
    script = <<-EOS
	if (id == window-><%= self.name() %>.id) {
		ret = cact_par(window-><%= self.name() %>.id, wev);
		if ((ret & 0x5000) != 0x5000) {
			return;
		}
		evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_PUSH;
		return;
	}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMISerialBoxParts < HMIParts
  def calc_serialbox_formatlength()
    l = 0;
    @yaml["fields"].each { |field|
      case field["type"]
      when "number"
        l += 3; # specify, max, min
      when "items"
        l += 1; # specify
        field["items"].each { |item| # item text
          l+= 1 + calc_euc_to_TCArray_length(item["text"]);
        }
        l += 1; # TNULL
      when "fixed"
        l += calc_euc_to_TCArray_length(field["text"]);
      end
    }
    l += 1; # for last TNULL
    return l
  end
  def calc_serialbox_fieldsnumber()
    l = 0;
    @yaml["fields"].each { |field|
      case field["type"]
      when "number", "items"
        l += 1;
      when "fixed"
        # not variable field.
      end
    }
    return l
  end
  def is_need_eventbreak()
    return true
  end

  def generate_header_eventtype_enumulate(main_name, window_name)
    script = <<-EOS
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_prototypes(window_name)
    script = <<-EOS
struct <%= window_name %>_<%= self.name() %>value_t_ {
<%- @yaml["fields"].each do |field| -%>
<%- case field["type"] -%>
<%- when "number" -%>
	W <%= field["name"] %>;
<%- when "items" -%>
	enum {
	<%- field["items"].each do |item| -%>
		<%= window_name.upcase %>_<%= self.name().upcase %>_<%= field["name"].upcase %>VALUE_<%= item["name"].upcase %>,
	<%- end -%>
	} <%= field["name"] %>;
<%- end -%>
<%- end -%>
};
typedef struct <%= window_name %>_<%= self.name() %>value_t_ <%= window_name %>_<%= self.name() %>value_t;
IMPORT W <%= window_name %>_set<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name %>_<%= self.name() %>value_t *value);
IMPORT W <%= window_name %>_get<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name %>_<%= self.name() %>value_t *value);
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct(window_name)
    script = <<-EOS
	struct {
		PAID id;
		<% if self.is_databox_specify_argument() %>W dnum;<% end %>
		TC format[<%= self.calc_serialbox_formatlength() %>];
		W cv[<%= self.calc_serialbox_fieldsnumber() %>];
		Bool nextaction;
	} <%= self.name() %>;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_functions(main_name, window_name)
    script = <<-EOS
EXPORT W <%= window_name %>_set<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name %>_<%= self.name() %>value_t *value)
{
	<%- j = 0 -%>
	<%- @yaml["fields"].each do |field| -%>
	<%- case field["type"] -%>
	<%- when "number" -%>
	if ((<%= field["min"] %> <= value-><%= field["name"] %>)&&(value-><%= field["name"] %> <= <%= field["max"] %>)) {
		window-><%= self.name() %>.cv[<%= j %>] = value-><%= field["name"] %>;<%- j+=1 %>
	}
	<%- when "items" -%>
	if ((0 <= value-><%= field["name"] %>)&&(value-><%= field["name"] %> < <%= field["items"].length %>)) {
		window-><%= self.name() %>.cv[<%= j %>] = value-><%= field["name"] %>;<%- j+=1 %>
	}
	<%- end -%>
	<%- end -%>
	if (window->wid < 0) {
		return 0;
	}

	return cset_val(window-><%= self.name() %>.id, <%= self.calc_serialbox_fieldsnumber() %>, (W*)(window-><%= self.name() %>.cv));
}

EXPORT W <%= window_name %>_get<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name %>_<%= self.name() %>value_t *value)
{
	W err;

	if (window->wid > 0) {
		err = cget_val(window-><%= self.name() %>.id, <%= self.calc_serialbox_fieldsnumber() %>, (W*)(window-><%= self.name() %>.cv));
		if (err < 0) {
			return err;
		}
	}

	<%- j = 0 -%>
	<%- @yaml["fields"].each do |field| -%>
	<%- case field["type"] -%>
	<%- when "number", "items" -%>
	value-><%= field["name"] %> = window-><%= self.name() %>.cv[<%= j %>];<%- j+=1 %>
	<%- end -%>
	<%- end -%>

	return 0;
}

LOCAL VOID <%= window_name %>_action<%= self.name() %>(<%= window_name %>_t *window, WEVENT *wev, <%= main_name %>event_t *evt)
{
	W i;

	i = cact_par(window-><%= self.name() %>.id, wev);
	if (i & 0x2000) {
		window-><%= self.name() %>.nextaction = True;
		<%= window_name %>_setflag(window, <%= window_name.upcase %>_FLAG_PARTS_OTHEREVENT);
		wugt_evt(wev);
		return;
	}
	window-><%= self.name() %>.nextaction = False;
	if (i & 0x1000) {
		cget_val(window-><%= self.name() %>.id, <%= self.calc_serialbox_fieldsnumber() %>, (W*)(window-><%= self.name() %>.cv));
		/* TODO: export event */
	}
	switch (i & 7) {
		case	P_BUT:
			wugt_evt(wev);
			break;
		case	P_TAB:
		case	P_NL:
		case	P_END:
		case	P_MOVE:
		case	P_COPY:
			break;
	}
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_in_new()
    script = <<-EOS
	window-><%= self.name() %>.id = -1;
	<%- if self.is_databox_specify_argument() -%>
	window-><%= self.name() %>.dnum = dnum_<%= self.name() %>;
	<%- end -%>
	<%- l = 0 -%>
	<%- @yaml["fields"].each do |field| -%>
	<%- case field["type"] -%>
	<%- when "number" -%>
	window-><%= self.name() %>.format[<%= l %>] = 0x0e00 | <%= field["chars"] %>;<%- l+=1 %>
	window-><%= self.name() %>.format[<%= l %>] = <%= field["min"] %>;<%- l+=1 %>
	window-><%= self.name() %>.format[<%= l %>] = <%= field["max"] %>;<%- l+=1 %>
	<%- when "items" -%>
	window-><%= self.name() %>.format[<%= l %>] = 0x0800 | <%= field["chars"] %>;<%- l+=1 %>
	  <%- field["items"].each do |item| -%>
	window-><%= self.name() %>.format[<%= l %>] = MC_STR;<%- l+=1 %>
	    <%- conv_euc_to_TCArray(item["text"]).each do |ch| -%>
	window-><%= self.name() %>.format[<%= l %>] = 0x<%= ch.to_s(16) %>;<%- l+=1 %>
	    <%- end -%>
	  <%- end -%>
	window-><%= self.name() %>.format[<%= l %>] = TNULL;<%- l+=1 %>
	<%- when "fixed" -%>
	  <%- conv_euc_to_TCArray(field["text"]).each do |ch| -%>
	window-><%= self.name() %>.format[<%= l %>] = 0x<%= ch.to_s(16) %>;<%- l+=1 %>
	  <%- end -%>
	<%- end -%>
	<%- end -%>
	window-><%= self.name() %>.format[<%= l %>] = TNULL;<%- l+=1 %>
	<%- l = 0 -%>
	<%- @yaml["fields"].each do |field| -%>
	<%- case field["type"] -%>
	<%- when "number" -%>
	window-><%= self.name() %>.cv[<%= l %>] = <%= field["min"] %>;<%- l+=1 %>
	<%- when "items" -%>
	window-><%= self.name() %>.cv[<%= l %>] = 0;<%- l+=1 %>
	<%- end -%>
	<%- end -%>
	window-><%= self.name() %>.nextaction = False;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_direct()
    script = "ccre_sbx(wid, SB_PARTS|P_DISP, &r, window-><%= self.name() %>.format, <%= self.calc_serialbox_fieldsnumber() %>, window-><%= self.name() %>.cv, NULL)"

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_loadvalue_in_open()
    script = <<-EOS
	cset_val(window-><%= self.name() %>.id, <%= self.calc_serialbox_fieldsnumber() %>, (W*)(window-><%= self.name() %>.cv));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_savevalue_in_close()
    script = <<-EOS
	err = cget_val(window-><%= self.name() %>.id, <%= self.calc_serialbox_fieldsnumber() %>, (W*)(window-><%= self.name() %>.cv));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_action_in_butdnwork(main_name, window_name)
    script = <<-EOS
	if (id == window-><%= self.name() %>.id) {
		<%= window_name %>_action<%= self.name() %>(window, wev, evt);
		return;
	}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMISwitchSelectorParts < HMIParts
  def calc_formatlength()
    l = 0;
    @yaml["fields"].each { |field|
      l+= 1 + calc_euc_to_TCArray_length(field["text"]);
    }
    l += 1; # for last TNULL
    return l
  end
  def is_need_eventbreak()
    return false
  end

  def generate_header_eventtype_enumulate(main_name, window_name)
    script = <<-EOS
	<%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_CHANGE,
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventstruct_definition(main_name, window_name)
    script = <<-EOS
struct <%= window_name %>_eventdata_<%= self.name() %>_change_t_ {
	<%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T value;
};
typedef struct <%= window_name %>_eventdata_<%= self.name() %>_change_t_ <%= window_name %>_eventdata_<%= self.name() %>_change_t;

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_eventunion_member(main_name, window_name)
    script = <<-EOS
		<%= window_name %>_eventdata_<%= self.name() %>_change_t <%= window_name %>_<%= self.name() %>_change;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_prototypes(window_name)
    script = <<-EOS
enum <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T_ {
	<%= window_name.upcase %>_<%= self.name().upcase %>VALUE_NOSEL = 0,
	<%- @yaml["fields"].each do |field| -%>
	<%= window_name.upcase %>_<%= self.name().upcase %>VALUE_<%= field["name"].upcase %>,
	<%- end -%>
};
typedef enum <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T_ <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T;
IMPORT W <%= window_name %>_set<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T value);
IMPORT W <%= window_name %>_get<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T *value);
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct(window_name)
    script = <<-EOS
	struct {
		PAID id;
		<%- if self.is_databox_use() -%>
		W dnum;
		<%- else -%>
		TC format[<%= self.calc_formatlength()%>];
		<%- end -%>
		<%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T value;
	} <%= self.name() %>;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_functions(main_name, window_name)
    script = <<-EOS
EXPORT W <%= window_name %>_set<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T value)
{
	W val;

	val = window-><%= self.name() %>.value = value;
	if (window->wid < 0) {
		return 0;
	}

	return cset_val(window-><%= self.name() %>.id, 1, (W*)(&val));
}

EXPORT W <%= window_name %>_get<%= self.name() %>value(<%= window_name %>_t *window, <%= window_name.upcase %>_<%= self.name().upcase %>VALUE_T *value)
{
	W val,err;

	if (window->wid > 0) {
		err = cget_val(window-><%= self.name() %>.id, 1, (W*)(&val));
		if (err < 0) {
			return err;
		}
	}
	*value = window-><%= self.name() %>.value = val;

	return 0;
}

LOCAL VOID <%= window_name %>_action<%= self.name() %>(<%= window_name %>_t *window, WEVENT *wev, <%= main_name %>event_t *evt)
{
	W i;

	i = cact_par(window-><%= self.name() %>.id, wev);
	if ((i & 0x5000) != 0x5000) {
		return;
	}
	window-><%= self.name() %>.value = i & 0xfff;
	evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= window_name.upcase %>_PARTS_<%= self.name().upcase %>_CHANGE;
	evt->data.<%= window_name %>_<%= self.name() %>_change.value = window-><%= self.name() %>.value;
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_in_new()
    script = <<-EOS
	window-><%= self.name() %>.id = -1;
	<%- if self.is_databox_use() -%>
	window-><%= self.name() %>.dnum = dnum_<%= self.name() %>;
	<%- else -%>
	  <%- l = 0 -%>
	  <%- @yaml["fields"].each do |field| -%>
	window-><%= self.name() %>.format[<%= l %>] = MC_STR;<%- l+=1 %>
	    <%- conv_euc_to_TCArray(field["text"]).each do |ch| -%>
	window-><%= self.name() %>.format[<%= l %>] = 0x<%= ch.to_s(16) %>;<%- l+=1 %>
	    <%- end -%>
	  <%- end -%>
	window-><%= self.name() %>.format[<%= l %>] = TNULL;<%- l+=1 %>
	<%- end -%>
	window-><%= self.name() %>.value = 0;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_create_systemcall_direct()
    script = "ccre_sel(wid, WS_PARTS|P_DISP, &r, window-><%= self.name() %>.value, window-><%= self.name() %>.format, NULL)"

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_loadvalue_in_open()
    script = <<-EOS
	cset_val(window-><%= self.name() %>.id, 1, (W*)(&window-><%= self.name() %>.value));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_savevalue_in_close()
    script = <<-EOS
	err = cget_val(window-><%= self.name() %>.id, 1, (W*)(&window-><%= self.name() %>.value));
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_action_in_butdnwork(main_name, window_name)
    script = <<-EOS
	if (id == window-><%= self.name() %>.id) {
		<%= window_name %>_action<%= self.name() %>(window, wev, evt);
		return;
	}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMIFixedTextParts < HMIParts
  def is_need_eventbreak()
    return false
  end
  def is_use_rect_in_open()
    return false
  end

  def generate_initialize_in_new()
    return ""
  end
  def generate_create_in_open()
    return ""
  end
  def generate_draw_in_draw(main_name, window_name)
    script = <<-EOS
	gdra_stp(window->gid, <%= self.rect_left() %>, <%= self.rect_bottom() %>, (TC[]){<%= self.text_array() %>}, <%= self.text_array_length() %>, G_STORE);
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

def generate_parts(type, a)
  case type
  when "textbox"
    return HMITextBoxParts.new(a);
  when "secrettextbox"
    return HMISecretTextBoxParts.new(a);
  when "textmomentallyswitch"
    return HMITextMomentallySwitchParts.new(a);
  when "serialbox"
    return HMISerialBoxParts.new(a);
  when "numberbox"
    return HMINumberBoxParts.new(a);
  when "switchselector"
    return HMISwitchSelectorParts.new(a);
  when "fixedtext"
    return HMIFixedTextParts.new(a)
  end
end

class HMIWindow
  attr_accessor :yaml, :parts
  def initialize(yaml)
    @yaml = yaml;
    @parts = Array.new();
    if yaml["parts"] != nil
      yaml["parts"].each { |a|
        w = generate_parts(a["type"], a)
        @parts.push(w);
      };
    end
  end
  def struct_name()
    @yaml["window_name"]
  end
  def is_attr_xxx(name)
    if @yaml["attributes"] == nil || @yaml["attributes"].empty?
      false
    else
      @yaml["attributes"].any? { |item|
        item == name;
      };
    end
  end
  def is_exportevent_xxx(name)
    if @yaml["export_events"] == nil || @yaml["export_events"].empty?
      false
    else
      @yaml["export_events"].any? { |item|
        item == name;
      };
    end
  end
  def is_attr_scrollable()
    self.is_attr_xxx("scrollable");
  end
  def is_attr_resizable()
    self.is_attr_xxx("resizable");
  end
  def is_attr_opencheckable()
    self.is_attr_xxx("opencheckable");
  end
  def is_attr_subwindow()
    self.is_attr_xxx("subwindow");
  end
  def is_attr_alwaysopen()
    self.is_attr_xxx("alwaysopen");
  end
  def is_exportevent_draw()
    self.is_exportevent_xxx("draw");
  end
  def is_exportevent_butdn()
    self.is_exportevent_xxx("butdn");
  end
  def is_exportevent_paste()
    self.is_exportevent_xxx("paste");
  end
  def is_exportevent_close()
    self.is_exportevent_xxx("close");
  end
  def is_exportevent_switch()
    self.is_exportevent_xxx("switch");
  end
  def is_exportevent_mousemove()
    self.is_exportevent_xxx("mousemove");
  end
  def is_exportevent_scroll()
    self.is_exportevent_xxx("scroll");
  end
  def is_exist_controllparts()
    @parts.length > 0;
  end
  def is_exist_use_rect_in_open()
    @parts.any? { |item|
      item.is_use_rect_in_open();
    };
  end
  def is_need_flag()
    true # tmp
  end
  def get_window_width()
    @yaml["size"]["h"]
  end
  def get_window_height()
    @yaml["size"]["v"]
  end
  def get_window_title()
    @yaml["title"]
  end
  def is_exist_eventbreak_parts()
      @parts.any? { |item|
        item.is_need_eventbreak();
      };
  end
  def get_window_parent()
    @yaml["parent"]
  end
  def get_window_title_max_length()
    len = @yaml["title_max_length"];
    if len == nil
      return 128
    end
    return len
  end

  def generate_header_eventtype_enumulate(main_name)
    script = <<-EOS
	<%- if self.is_exportevent_draw() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_DRAW,
	<%- end -%>
	<%- if self.is_attr_resizable() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_RESIZE,
	<%- end -%>
	<%- if self.is_exportevent_close() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_CLOSE,
	<%- end -%>
	<%- if self.is_exportevent_butdn() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_BUTDN,
	<%- end -%>
	<%- if self.is_exportevent_paste() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_PASTE,
	<%- end -%>
	<%- if self.is_exportevent_switch() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_SWITCH,
	<%- end -%>
	<%- if self.is_exportevent_mousemove() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_MOUSEMOVE,
	<%- end -%>
	<%- if self.is_exportevent_scroll() and self.is_attr_scrollable() -%>
	<%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_SCROLL,
	<%- end -%>
	<%- @parts.each do |p| -%><%= p.generate_header_eventtype_enumulate(main_name, self.struct_name()) %><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_partsevent_struct(main_name)
    script = <<-EOS
	<%- @parts.each do |p| -%><%= p.generate_header_eventstruct_definition(main_name, self.struct_name()) %><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header_partsevent_unionmember(main_name)
    script = <<-EOS
	<%- @parts.each do |p| -%><%= p.generate_header_eventunion_member(main_name, self.struct_name()) %><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_prototypes()
    script = <<-EOS
typedef struct <%= self.struct_name() %>_t_ <%= self.struct_name() %>_t;
<%- if self.is_attr_scrollable() -%>
typedef VOID (*<%= self.struct_name() %>_scrollcalback)(VP arg, W dh, W dv);
<%- end -%>

<%- if self.is_attr_scrollable() -%>
IMPORT VOID <%= self.struct_name() %>_scrollbyvalue(<%= self.struct_name() %>_t *window, W dh, W dv);
IMPORT W <%= self.struct_name() %>_setdrawrect(<%= self.struct_name() %>_t *window, W l, W t, W r, W b);
IMPORT W <%= self.struct_name() %>_setworkrect(<%= self.struct_name() %>_t *window, W l, W t, W r, W b);
IMPORT W <%= self.struct_name() %>_scrollworkarea(<%= self.struct_name() %>_t *window, W dh, W dv);
IMPORT W <%= self.struct_name() %>_getworkrect(<%= self.struct_name() %>_t *window, RECT *r);
<%- end -%>
<%- if self.is_attr_opencheckable() -%>
IMPORT Bool <%= self.struct_name() %>_isopen(<%= self.struct_name() %>_t *window);
<%- end -%>
<%- if self.is_exportevent_paste() -%>
IMPORT VOID <%= self.struct_name() %>_responsepasterequest(<%= self.struct_name() %>_t *window, W nak, PNT *pos);
<%- end -%>
<%- if self.is_exportevent_draw() -%>
IMPORT W <%= self.struct_name() %>_startredisp(<%= self.struct_name() %>_t *window, RECT *r);
IMPORT W <%= self.struct_name() %>_endredisp(<%= self.struct_name() %>_t *window);
IMPORT W <%= self.struct_name() %>_eraseworkarea(<%= self.struct_name() %>_t *window, RECT *r);
<%- end -%>
IMPORT W <%= self.struct_name() %>_requestredisp(<%= self.struct_name() %>_t *window);
<%- if self.is_exportevent_butdn() -%>
IMPORT GID <%= self.struct_name() %>_startdrag(<%= self.struct_name() %>_t *window);
IMPORT W <%= self.struct_name() %>_getdrag(<%= self.struct_name() %>_t *window, PNT *pos, WID *wid, PNT *pos_butup);
IMPORT VOID <%= self.struct_name() %>_enddrag(<%= self.struct_name() %>_t *window);
<%- end -%>
IMPORT GID <%= self.struct_name() %>_getGID(<%= self.struct_name() %>_t *window);
IMPORT WID <%= self.struct_name() %>_getWID(<%= self.struct_name() %>_t *window);
IMPORT W <%= self.struct_name() %>_settitle(<%= self.struct_name() %>_t *window, TC *title);
IMPORT Bool <%= self.struct_name() %>_isactive(<%= self.struct_name() %>_t *window);
<%- if !self.is_attr_alwaysopen() -%>
IMPORT W <%= self.struct_name() %>_open(<%= self.struct_name() %>_t *window);
IMPORT VOID <%= self.struct_name() %>_close(<%= self.struct_name() %>_t *window);
<%- end -%>
<%- @parts.each do |p| -%><%= p.generate_prototypes(self.struct_name()) %><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_arguments()
    script = <<-EOS
<%- if self.is_attr_resizable() -%>RECT *r<% else %>PNT *p<% end %>, <%- if self.is_attr_subwindow() -%><%= self.get_window_parent() %>_t *parent<%- else -%>WID parent<%- end -%>, TC *title, PAT *bgpat<%- @parts.each do |p| -%><% if p.is_databox_specify_argument() %>, W dnum_<%= p.name() %><% end %><%- end -%><%- -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_initialize_arguments_values()
    script = <<-EOS
<%- if self.is_attr_resizable() -%>r<% else %>p<% end %>, parent, title, bgpat<%- @parts.each do |p| -%><% if p.is_databox_specify_argument() %>, dnum_<%= p.name() %><% end %><%- end -%><%- -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_struct()
    script = <<-EOS
struct <%= self.struct_name() %>_t_ {
	<%- if self.is_need_flag() -%>
	UW flag;
	<%- end -%>
	WID wid;
	GID gid;
	<%- if self.is_attr_subwindow() -%>
	<%= self.get_window_parent() %>_t *parent;
	<%- else -%>
	WID parent;
	<%- end -%>
	RECT r;
	PAT bgpat;
	<%- if self.is_attr_scrollable() -%>
	hmi_windowscroll_t wscr;
	<%- end -%>
	<%- if !self.is_attr_alwaysopen() -%>
	TC title[<%= self.get_window_title_max_length() %>+1];
	<%- end -%>
	WEVENT savedwev;
	<%- @parts.each do |p| -%><%= p.generate_source_struct(self.struct_name()) %><%- end -%>
};

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_functions(main_name)
    script = <<-EOS
<%- if self.is_need_flag() -%>
#define <%= self.struct_name().upcase %>_FLAG_DRAWREQUEST 0x00000001
<%- if self.is_attr_scrollable() -%>
#define <%= self.struct_name().upcase %>_FLAG_RSCROLLING 0x00000002
#define <%= self.struct_name().upcase %>_FLAG_BSCROLLING 0x00000004
<%- end -%>
<%- if self.is_exist_eventbreak_parts -%>
#define <%= self.struct_name().upcase %>_FLAG_PARTS_OTHEREVENT 0x0000000f
#define <%= self.struct_name().upcase %>_FLAG_PARTS_NEXTACTION 0x00000010
<%- end -%>

#define <%= self.struct_name() %>_setflag(window, flagx) (window)->flag = (window)->flag | (flagx)
#define <%= self.struct_name() %>_clearflag(window, flagx) (window)->flag = (window)->flag & ~(flagx)
#define <%= self.struct_name() %>_issetflag(window, flagx) (((window)->flag & (flagx)) == 0 ? False : True)

<%- end -%>
<%- @parts.each do |p| -%><%= p.generate_source_functions(main_name, self.struct_name()) %><%- end -%>
<%- if self.is_attr_scrollable() -%>
EXPORT VOID <%= self.struct_name() %>_scrollbyvalue(<%= self.struct_name() %>_t *window, W dh, W dv)
{
	hmi_windowscroll_scrollworkrect(&window->wscr, dh, dv);
}

EXPORT W <%= self.struct_name() %>_setdrawrect(<%= self.struct_name() %>_t *window, W l, W t, W r, W b)
{
	return hmi_windowscroll_setdrawrect(&window->wscr, l, t, r, b);
}

EXPORT W <%= self.struct_name() %>_setworkrect(<%= self.struct_name() %>_t *window, W l, W t, W r, W b)
{
	return hmi_windowscroll_setworkrect(&window->wscr, l, t, r, b);
}

EXPORT W <%= self.struct_name() %>_scrollworkarea(<%= self.struct_name() %>_t *window, W dh, W dv)
{
	W err;
	err = wscr_wnd(window->wid, NULL, dh, dv, W_MOVE|W_RDSET);
	if (err < 0) {
		return err;
	}
	if ((err & W_RDSET) != 0) {
		<%= self.struct_name() %>_setflag(window, <%= self.struct_name().upcase %>_FLAG_DRAWREQUEST);
	}
	return 0;
}

EXPORT W <%= self.struct_name() %>_getworkrect(<%= self.struct_name() %>_t *window, RECT *r)
{
	return wget_wrk(window->wid, r);
}

<%- end -%>
<%- if self.is_attr_opencheckable() -%>
EXPORT Bool <%= self.struct_name() %>_isopen(<%= self.struct_name() %>_t *window)
{
	if (window->wid < 0) {
		return False;
	}
	return True;
}

<%- end -%>
<%- if self.is_exportevent_paste() -%>
EXPORT VOID <%= self.struct_name() %>_responsepasterequest(<%= self.struct_name() %>_t *window, W nak, PNT *pos)
{
	if (pos != NULL) {
		window->savedwev.r.r.p.rightbot.x = pos->x;
		window->savedwev.r.r.p.rightbot.y = pos->y;
	}
	wrsp_evt(&window->savedwev, nak);
}

<%- end -%>
<%- if self.is_exportevent_draw() -%>
EXPORT W <%= self.struct_name() %>_startredisp(<%= self.struct_name() %>_t *window, RECT *r)
{
	return wsta_dsp(window->wid, r, NULL);
}

EXPORT W <%= self.struct_name() %>_endredisp(<%= self.struct_name() %>_t *window)
{
	return wend_dsp(window->wid);
}

EXPORT W <%= self.struct_name() %>_eraseworkarea(<%= self.struct_name() %>_t *window, RECT *r)
{
	return wera_wnd(window->wid, r);
}

<%- else -%>
LOCAL VOID <%= self.struct_name() %>_draw(<%= self.struct_name() %>_t *window, RECT *r)
{
	cdsp_pwd(window->wid, r, P_RDISP);
	<%- @parts.each do |p| -%><%= p.generate_draw_in_draw(main_name, self.struct_name()) %><%- end -%>
}

LOCAL VOID <%= self.struct_name() %>_redisp(<%= self.struct_name() %>_t *window)
{
	RECT r;
	do {
		if (wsta_dsp(window->wid, &r, NULL) == 0) {
			break;
		}
		wera_wnd(window->wid, &r);
		<%= self.struct_name() %>_draw(window, &r);
	} while (wend_dsp(window->wid) > 0);
}

<%- end -%>
EXPORT W <%= self.struct_name() %>_requestredisp(<%= self.struct_name() %>_t *window)
{
	return wreq_dsp(window->wid);
}

<%- if self.is_exportevent_butdn() -%>
EXPORT GID <%= self.struct_name() %>_startdrag(<%= self.struct_name() %>_t *window)
{
	return wsta_drg(window->wid, 0);
}

EXPORT W <%= self.struct_name() %>_getdrag(<%= self.struct_name() %>_t *window, PNT *pos, WID *wid, PNT *pos_butup)
{
	W etype;

	etype = wget_drg(pos, &window->savedwev);
	*wid = window->savedwev.s.wid;
	if (etype == EV_BUTUP) {
		*pos_butup = window->savedwev.s.pos;
	}

	return etype;
}

EXPORT VOID <%= self.struct_name() %>_enddrag(<%= self.struct_name() %>_t *window)
{
	wend_drg();
}

<%- end -%>
EXPORT GID <%= self.struct_name() %>_getGID(<%= self.struct_name() %>_t *window)
{
	return wget_gid(window->wid);
}

EXPORT WID <%= self.struct_name() %>_getWID(<%= self.struct_name() %>_t *window)
{
	return window->wid;
}

EXPORT W <%= self.struct_name() %>_settitle(<%= self.struct_name() %>_t *window, TC *title)
{
	<%- if !self.is_attr_alwaysopen() -%>
	tc_strncpy(window->title, title, <%= self.get_window_title_max_length() %>);
	window->title[<%= self.get_window_title_max_length() %>] = TNULL;
	return wset_tit(window->wid, -1, window->title, 0);
	<%- else -%>
	return wset_tit(window->wid, -1, title, 0);
	<%- end -%>
}

EXPORT Bool <%= self.struct_name() %>_isactive(<%= self.struct_name() %>_t *window)
{
	WID wid;
	wid = wget_act(NULL);
	if (window->wid == wid) {
		return True;
	}
	return False;
}

LOCAL VOID <%= self.struct_name() %>_butdnwork(<%= self.struct_name() %>_t *window, WEVENT *wev, <%= main_name %>event_t *evt)
{
	<%- if self.is_exist_controllparts() -%>
	PAID id;
	W ret;

	ret = cfnd_par(window->wid, wev->s.pos, &id);
	if (ret <= 0) {
		return;
	}
	<%- @parts.each do |p| -%><%= p.generate_action_in_butdnwork(main_name, self.struct_name()) %><%- end -%>
	<%- end -%>
	<%- if self.is_exportevent_butdn() -%>
	evt->type = <%= main_name.upcase %>EVENT_TYPE_<%= self.struct_name().upcase %>_BUTDN;
	evt->data.<%= self.struct_name() %>_butdn.type = wchk_dck(wev->s.time);
	evt->data.<%= self.struct_name() %>_butdn.pos = wev->s.pos;
	memcpy(&window->savedwev, wev, sizeof(WEVENT));
	<%- end -%>
}

<%- if self.is_attr_resizable() -%>
LOCAL VOID <%= self.struct_name() %>_resize(<%= self.struct_name() %>_t *window, SIZE *sz)
{
	RECT work;
	Bool workchange = False;

	wget_wrk(window->wid, &work);
	if (work.c.left != 0) {
		work.c.left = 0;
		workchange = True;
	}
	if (work.c.top != 0) {
		work.c.top = 0;
		workchange = True;
	}
	wset_wrk(window->wid, &work);
	gset_vis(window->gid, work);

	if (workchange == True) {
		wera_wnd(window->wid, NULL);
		wreq_dsp(window->wid);
	}

	sz->v = work.c.bottom - work.c.top;
	sz->h = work.c.right - work.c.left;
}

<%- end -%>
<% if self.is_attr_alwaysopen() %>LOCAL<% else %>EXPORT<% end %> W <%= self.struct_name() %>_open(<%= self.struct_name() %>_t *window<% if self.is_attr_alwaysopen() %>, TC *title<% end %>)
{
	WID wid;
	<%- if self.is_exist_use_rect_in_open() -%>
	RECT r;
	<%- end -%>

	if (window->wid > 0) {
		return 0;
	}

	wid = wopn_wnd(WA_STD<% if self.is_attr_subwindow() %>|WA_SUBW<% end %><% if self.is_attr_resizable() %>|WA_SIZE|WA_HHDL|WA_VHDL<% end %><% if self.is_attr_scrollable() %>|WA_BBAR|WA_RBAR<% end %>, <% if self.is_attr_subwindow() %>window->parent->wid<% else %>window->parent<% end %>, &(window->r), NULL, 2, <% if !self.is_attr_alwaysopen() %>window->title<%- else -%>title<%- end -%>, &window->bgpat, NULL);
	if (wid < 0) {
		DP_ER("wopn_wnd: subjectoption error", wid);
		return wid;
	}
	window->wid = wid;
	window->gid = wget_gid(wid);
	<%- if self.is_attr_scrollable() -%>
	hmi_windowscroll_settarget(&window->wscr, wid);
	<%- end -%>

	<%- @parts.each do |p| -%><%= p.generate_create_in_open() %><%- end -%>

	wreq_dsp(wid);

	return 0;
}

<%- if !self.is_attr_alwaysopen() -%>
EXPORT VOID <%= self.struct_name() %>_close(<%= self.struct_name() %>_t *window)
{
	WDSTAT stat;
	W err;

	if (window->wid < 0) {
		return;
	}

	stat.attr = WA_STD;
	err = wget_sts(window->wid, &stat, NULL);
	if (err >= 0) {
		window->r = stat.r;
	}
	<%- @parts.reverse_each do |p| -%><%= p.generate_savevalue_in_close() %><%- end -%>
	<%- if self.is_exist_controllparts() -%>
	cdel_pwd(window->wid, NOCLR);
	<%- end -%>
	wcls_wnd(window->wid, CLR);
	window->wid = -1;
	window->gid = -1;
}

<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_checkpartsflag_in_checkflag(main_name)
    script = <<-EOS
	<%- if self.is_exist_eventbreak_parts() -%>
	if (<%= self.struct_name() %>_issetflag(hmi-><%= self.struct_name() %>, <%= self.struct_name().upcase %>_FLAG_PARTS_OTHEREVENT) == True) {
		<%= self.struct_name() %>_clearflag(hmi-><%= self.struct_name() %>, <%= self.struct_name().upcase %>_FLAG_PARTS_OTHEREVENT);
		<%= self.struct_name() %>_setflag(hmi-><%= self.struct_name() %>, <%= self.struct_name().upcase %>_FLAG_PARTS_NEXTACTION);
		return False;
	} else if (<%= self.struct_name() %>_issetflag(hmi-><%= self.struct_name() %>, <%= self.struct_name().upcase %>_FLAG_PARTS_NEXTACTION) == True) {
		<%= self.struct_name() %>_clearflag(hmi-><%= self.struct_name() %>, <%= self.struct_name().upcase %>_FLAG_PARTS_NEXTACTION);
	<%- self.parts.each do |p| -%>
	<%- if p.is_need_eventbreak() -%>
		if (hmi-><%= self.struct_name() %>-><%= p.name() %>.nextaction == True) {
			hmi->wev.s.type = EV_NULL;
			<%= self.struct_name() %>_action<%= p.name() %>(hmi-><%= self.struct_name() %>, &hmi->wev, &hmi->evt);
			return True;
		}
	<%- end -%>
	<%- end -%>
	}
	<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_new_del_functions()
    script = <<-EOS
EXPORT <%= self.struct_name() %>_t* <%= self.struct_name() %>_new(<%= self.generate_initialize_arguments() %>)
{
	<%= self.struct_name() %>_t *window;
	<%- if self.is_attr_scrollable() -%>
	W err;
	<%- end -%>

	window = (<%= self.struct_name() %>_t*)malloc(sizeof(<%= self.struct_name() %>_t));
	if (window == NULL) {
		return NULL;
	}
	<%- if self.is_attr_scrollable() -%>
	window->flag = 0;
	<%- end -%>
	window->wid = -1;
	window->gid = -1;
	window->parent = parent;
	<%- if self.is_attr_resizable() -%>
	window->r = *r;
	<%- else -%>
	window->r.c.left = p->x;
	window->r.c.top = p->y;
	window->r.c.right = p->x + <%= self.get_window_width() %>;
	window->r.c.bottom = p->y + <%= self.get_window_height() %>;
	<%- end -%>
	<%- if self.is_attr_scrollable() -%>
	err = hmi_windowscroll_initialize(&window->wscr, window->wid);
	if (err < 0) {
		free(window);
		return NULL;
	}
	<%- end -%>
	<%- if !self.is_attr_alwaysopen() -%>
	tc_strset(window->title, TNULL, <%= self.get_window_title_max_length() %>+1);
	if (title != 0) {
		tc_strncpy(window->title, title, <%= self.get_window_title_max_length() %>);
	} else {
		<%- l = 0 -%>
		<%- conv_euc_to_TCArray(self.get_window_title()).each do |ch| -%>
		window->title[<%= l %>] = 0x<%= ch.to_s(16) %>;<%- l+=1 %>
		<%- end -%>
	}
	<%- end -%>
	<%- @parts.each do |p| -%><%= p.generate_initialize_in_new() %><%- end -%>

	<%- if self.is_attr_alwaysopen() -%>
	err = <%= self.struct_name() %>_open(window, title);
	if (err < 0) {
		<%- if self.is_attr_scrollable() -%>
		hmi_windowscroll_finalize(&window->wscr);
		<%- end -%>
		free(window);
		return NULL;
	}
	<%- end -%>

	return window;
}

LOCAL VOID <%= self.struct_name() %>_delete(<%= self.struct_name() %>_t *window)
{
	if (window->wid > 0) {
		<%- if self.is_exist_controllparts() -%>
		cdel_pwd(window->wid, NOCLR);
		<%- end -%>
		wcls_wnd(window->wid, CLR);
	}
	<%- if self.is_attr_scrollable() -%>
	hmi_windowscroll_finalize(&window->wscr);
	<%- end -%>
	free(window);
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class HMIData
  attr_accessor :yaml, :win, :name
  def initialize(yaml)
    @yaml = yaml;
    @win = Array.new();
    yaml["windows"].each { |a|
      w = HMIWindow.new(a);
      @win.push(w);
    };
  end
  def main_name()
    @yaml["generator"]["name"]
  end
  def lisence_header()
    @yaml["generator"]["lisence_header"]
  end
  def lisence_source()
    @yaml["generator"]["lisence_source"]
  end
  def filename_header()
    @yaml["generator"]["output_header"]
  end

  def generate_header_eventtype_enumulate()
    script = <<-EOS
enum {
	<%= self.main_name().upcase %>EVENT_TYPE_NONE,
	<%= self.main_name().upcase %>EVENT_TYPE_COMMON_MOUSEMOVE,
	<%= self.main_name().upcase %>EVENT_TYPE_COMMON_KEYDOWN,
	<%= self.main_name().upcase %>EVENT_TYPE_COMMON_MENU,
	<%= self.main_name().upcase %>EVENT_TYPE_COMMON_TIMEOUT,
	<%- @win.each do |w| -%><%= w.generate_header_eventtype_enumulate(self.main_name()) %><%- end -%>
};
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
  def generate_header_eventtype_struct()
    script = <<-EOS
struct <%= self.main_name() %>_eventdata_mousemove_t_ {
	PNT pos;
};
typedef struct <%= self.main_name() %>_eventdata_mousemove_t_ <%= self.main_name() %>_eventdata_mousemove_t;

struct <%= self.main_name() %>_eventdata_keydown_t_ {
	TC keycode;
	UH keytop;
	UW stat;
};
typedef struct <%= self.main_name() %>_eventdata_keydown_t_ <%= self.main_name() %>_eventdata_keydown_t;

struct <%= self.main_name() %>_eventdata_menu_t_ {
	PNT pos;
};
typedef struct <%= self.main_name() %>_eventdata_menu_t_ <%= self.main_name() %>_eventdata_menu_t;

struct <%= self.main_name() %>_eventdata_timeout_t_ {
	W code;
};
typedef struct <%= self.main_name() %>_eventdata_timeout_t_ <%= self.main_name() %>_eventdata_timeout_t;

<%- @win.each do |w| -%>
<%- if w.is_exportevent_draw() -%>
struct <%= w.struct_name() %>_eventdata_draw_t_ {
};
typedef struct <%= w.struct_name() %>_eventdata_draw_t_ <%= w.struct_name() %>_eventdata_draw_t;

<%- end -%>
<%- if w.is_attr_resizable() -%>
struct <%= w.struct_name() %>_eventdata_resize_t_ {
	SIZE work_sz;
};
typedef struct <%= w.struct_name() %>_eventdata_resize_t_ <%= w.struct_name() %>_eventdata_resize_t;

<%- end -%>
<%- if w.is_exportevent_close() -%>
struct <%= w.struct_name() %>_eventdata_close_t_ {
	Bool save;
};
typedef struct <%= w.struct_name() %>_eventdata_close_t_ <%= w.struct_name() %>_eventdata_close_t;

<%- end -%>
<%- if w.is_exportevent_butdn() -%>
struct <%= w.struct_name() %>_eventdata_butdn_t_ {
	W type;
	PNT pos;
};
typedef struct <%= w.struct_name() %>_eventdata_butdn_t_ <%= w.struct_name() %>_eventdata_butdn_t;

<%- end -%>
<%- if w.is_exportevent_paste() -%>
struct <%= w.struct_name() %>_eventdata_paste_t_ {
};
typedef struct <%= w.struct_name() %>_eventdata_paste_t_ <%= w.struct_name() %>_eventdata_paste_t;

<%- end -%>
<%- if w.is_exportevent_switch() -%>
struct <%= w.struct_name() %>_eventdata_switch_t_ {
};
typedef struct <%= w.struct_name() %>_eventdata_switch_t_ <%= w.struct_name() %>_eventdata_switch_t;

<%- end -%>
<%- if w.is_exportevent_mousemove() -%>
struct <%= w.struct_name() %>_eventdata_mousemove_t_ {
	PNT pos;
	UW stat;
};
typedef struct <%= w.struct_name() %>_eventdata_mousemove_t_ <%= w.struct_name() %>_eventdata_mousemove_t;

<%- end -%>
<%- if w.is_exportevent_scroll() and w.is_attr_scrollable() -%>
struct <%= w.struct_name() %>_eventdata_scroll_t_ {
	W dh;
	W dv;
};
typedef struct <%= w.struct_name() %>_eventdata_scroll_t_ <%= w.struct_name() %>_eventdata_scroll_t;

<%- end -%>
<%= w.generate_header_partsevent_struct(self.main_name()) %><%- -%>
<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
  def generate_header_eventtype_union()
    script = <<-EOS
struct <%= self.main_name() %>event_t_ {
	W type;
	union  {
		<%= self.main_name() %>_eventdata_mousemove_t common_mousemove;
		<%= self.main_name() %>_eventdata_keydown_t common_keydown;
		<%= self.main_name() %>_eventdata_menu_t common_menu;
		<%= self.main_name() %>_eventdata_timeout_t common_timeout;
<%- @win.each do |w| -%>
		<%- if w.is_exportevent_draw() -%>
		<%= w.struct_name() %>_eventdata_draw_t <%= w.struct_name() %>_draw;
		<%- end -%>
		<%- if w.is_attr_resizable() -%>
		<%= w.struct_name() %>_eventdata_resize_t <%= w.struct_name() %>_resize;
		<%- end -%>
		<%- if w.is_exportevent_close() -%>
		<%= w.struct_name() %>_eventdata_close_t <%= w.struct_name() %>_close;
		<%- end -%>
		<%- if w.is_exportevent_butdn() -%>
		<%= w.struct_name() %>_eventdata_butdn_t <%= w.struct_name() %>_butdn;
		<%- end -%>
		<%- if w.is_exportevent_paste() -%>
		<%= w.struct_name() %>_eventdata_paste_t <%= w.struct_name() %>_paste;
		<%- end -%>
		<%- if w.is_exportevent_switch() -%>
		<%= w.struct_name() %>_eventdata_switch_t <%= w.struct_name() %>_switch;
		<%- end -%>
		<%- if w.is_exportevent_mousemove() -%>
		<%= w.struct_name() %>_eventdata_mousemove_t <%= w.struct_name() %>_mousemove;
		<%- end -%>
		<%- if w.is_exportevent_scroll() and w.is_attr_scrollable() -%>
		<%= w.struct_name() %>_eventdata_scroll_t <%= w.struct_name() %>_scroll;
		<%- end -%>
		<%- -%><%= w.generate_header_partsevent_unionmember(self.main_name()) %><%- -%>
<%- end -%>
	} data;
};
typedef struct <%= self.main_name() %>event_t_ <%= self.main_name() %>event_t;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
  def generate_header_main_prototypes()
    script = <<-EOS
typedef struct <%= self.main_name() %>_t_ <%= self.main_name() %>_t;

IMPORT <%= self.main_name() %>_t* <%= self.main_name() %>_new();
IMPORT VOID <%= self.main_name() %>_delete(<%= self.main_name() %>_t *hmi);
IMPORT W <%= self.main_name() %>_getevent(<%= self.main_name() %>_t *hmi, <%= self.main_name() %>event_t **evt);
<%- @win.each do |w| -%>
IMPORT <%= w.struct_name() %>_t* <%= self.main_name() %>_new<%= w.struct_name() %>(<%= self.main_name() %>_t *hmi, <%= w.generate_initialize_arguments() %>);
<%- end -%>
<%- @win.each do |w| -%>
IMPORT VOID <%= self.main_name() %>_delete<%= w.struct_name() %>(<%= self.main_name() %>_t *hmi, <%= w.struct_name() %>_t *window);
<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_header()
    str = String.new();
    @win.each { |w|
      str += w.generate_prototypes() + "\n";
    };
    str += self.generate_header_eventtype_enumulate() + "\n";
    str += self.generate_header_eventtype_struct();
    str += self.generate_header_eventtype_union() + "\n";
    str += self.generate_header_main_prototypes() + "\n";
    str;
  end

  def generate_source_include_files()
    script = <<-EOS
#include    "<%= self.filename_header() %>"
#include    "hmi/hmi_windowscroll.h"

#include	<bstdio.h>
#include	<bstdlib.h>
#include	<tcode.h>
#include	<tstring.h>
#include	<btron/btron.h>
#include	<btron/hmi.h>
#include	<btron/vobj.h>

#if 1
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_mainloop_windowid_valuablelist()
    script = <<-EOS
<%- @win.each_with_index do |w, idx| -%>
<%= w.struct_name() %>_id<%- if idx != @win.length - 1 -%>, <%- end -%>
<%- end -%>
    EOS
    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_mainloop()
    script = <<-EOS
LOCAL VOID <%= self.main_name() %>_setswitchbutdnflag(<%= self.main_name() %>_t *hmi)
{
	hmi->flag = hmi->flag | <%= self.main_name().upcase %>_FLAG_SWITCHBUTDN;
}

LOCAL VOID <%= self.main_name() %>_clearswitchbutdnflag(<%= self.main_name() %>_t *hmi)
{
	hmi->flag = hmi->flag & ~<%= self.main_name().upcase %>_FLAG_SWITCHBUTDN;
}

LOCAL Bool <%= self.main_name() %>_issetswitchbutdnflag(<%= self.main_name() %>_t *hmi)
{
	if ((hmi->flag & <%= self.main_name().upcase %>_FLAG_SWITCHBUTDN) == 0) {
		return False;
	}
	return True;
}

<%- @win.each do |w| -%>
LOCAL Bool <%= self.main_name() %>_is<%= w.struct_name() %>WID(<%= self.main_name() %>_t *hmi, WID wid)
{
	if (hmi-><%= w.struct_name() %>->wid == wid) {
		return True;
	}
	return False;
}

<%- end -%>
LOCAL VOID <%= self.main_name() %>_weventnull(<%= self.main_name() %>_t *hmi, WEVENT *wev, <%= self.main_name() %>event_t *evt)
{
	cidl_par(wev->s.wid, &wev->s.pos);
	if (wev->s.cmd != W_WORK)
		return;		/*作業領域外*/
	if (wev->s.stat & ES_CMD)
		return;	/*命令キーが押されている*/
<%- @win.each do |w| -%>
	if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
		<%- if w.is_exist_controllparts() -%>
		cidl_par(wev->s.wid, &wev->s.pos);
		<%- end -%>
		<%- if w.is_exportevent_mousemove() -%>
		hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_MOUSEMOVE;
		hmi->evt.data.<%= w.struct_name() %>_mousemove.pos = wev->s.pos;
		hmi->evt.data.<%= w.struct_name() %>_mousemove.stat = wev->s.stat;
		<%- elsif !w.is_exist_controllparts() -%>
		gset_ptr(PS_SELECT, NULL, -1, -1);
		<%- end -%>
		return;
	}
<%- end -%>
	/*ウィンドウ外*/
	hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_COMMON_MOUSEMOVE;
	hmi->evt.data.common_mousemove.pos = wev->s.pos;
}

LOCAL VOID <%= self.main_name() %>_weventrequest(<%= self.main_name() %>_t *hmi, WEVENT *wev, <%= self.main_name() %>event_t *evt)
{
	switch (wev->g.cmd) {
	case	W_REDISP:	/*再表示要求*/
		<%- @win.each do |w| -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->g.wid) == True) {
			<%- if w.is_exportevent_draw() -%>
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_DRAW;
			<%- else -%>
			<%= w.struct_name() %>_redisp(hmi-><%= w.struct_name() %>);
			<%- end -%>
			break;
		}
		<%- end -%>
		break;
	case	W_PASTE:	/*貼込み要求*/
		<%- @win.each do |w| -%>
		<%- if w.is_exportevent_paste() -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->g.wid) == True) {
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_PASTE;
			memcpy(&hmi-><%= w.struct_name() %>->savedwev, wev, sizeof(WEVENT));
			break;
		}
		<%- end -%>
		<%- end -%>
		wrsp_evt(wev, 1); /*NACK*/
		break;
	case	W_DELETE:	/*保存終了*/
		wrsp_evt(wev, 0);	/*ACK*/
		<%- @win.each do |w| -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->g.wid) == True) {
			<%- if w.is_exportevent_close() -%>
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_CLOSE;
			evt->data.<%= w.struct_name() %>_close.save = True;
			<%- else -%>
			<%= w.struct_name() %>_close(hmi-><%= w.struct_name() %>);
			<%- end -%>
			break;
		}
		<%- end -%>
		break;
	case	W_FINISH:	/*廃棄終了*/
		wrsp_evt(wev, 0);	/*ACK*/
		<%- @win.each do |w| -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->g.wid) == True) {
			<%- if w.is_exportevent_close() -%>
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_CLOSE;
			evt->data.<%= w.struct_name() %>_close.save = False;
			<%- else -%>
			<%= w.struct_name() %>_close(hmi-><%= w.struct_name() %>);
			<%- end -%>
			break;
		}
		<%- end -%>
		break;
	}
}

LOCAL VOID <%= self.main_name() %>_weventbutdn(<%= self.main_name() %>_t *hmi, WEVENT *wev, <%= self.main_name() %>event_t *evt)
{
	W i, err;
	W dh, dv; /* TODO: add check for exist scrollable window */

	switch	(wev->s.cmd) {
	case	W_PICT:
		switch (wchk_dck(wev->s.time)) {
		case	W_DCLICK:
			<%- @win.each do |w| -%>
			if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
				<%- if w.is_exportevent_close() -%>
				evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_CLOSE;
				evt->data.<%= w.struct_name() %>_close.save = True; /* TODO: tmp value */
				<%- else -%>
				<%= w.struct_name() %>_close(hmi-><%= w.struct_name() %>);
				<%- end -%>
				return;
			}
			<%- end -%>
			return;
		case	W_PRESS:
			break;
		default:
			return;
		}
	case	W_FRAM:
	case	W_TITL:
		if (wmov_drg(wev, NULL) > 0) {
			<%- @win.each do |w| -%>
			if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
				<%- if w.is_exportevent_draw() -%>
				evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_DRAW;
				<%- else -%>
				<%= w.struct_name() %>_redisp(hmi-><%= w.struct_name() %>);
				<%- end -%>
				return;
			}
			<%- end -%>
		}
		return;
	case	W_LTHD:
	case	W_RTHD:
	case	W_LBHD:
	case	W_RBHD:
		switch (wchk_dck(wev->s.time)) {
		case	W_DCLICK:
			i = wchg_wnd(wev->s.wid, NULL, W_MOVE);
			break;
		case	W_PRESS:
			i = wrsz_drg(wev, NULL, NULL);
			break;
		default:
			return;
		}

		<%- @win.each do |w| -%>
		<%- if w.is_attr_resizable() -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_RESIZE;
			<%= w.struct_name() %>_resize(hmi-><%= w.struct_name() %>, &evt->data.<%= w.struct_name() %>_resize.work_sz);
			hmi_windowscroll_updatebar(&hmi-><%= w.struct_name() %>->wscr);
			if (i > 0) {
				<%= w.struct_name() %>_setflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_DRAWREQUEST);
			}
			return;
		}
		<%- end -%>
		<%- end -%>
		return;
	case	W_RBAR:
		<%- @win.each do |w| -%>
		<%- if w.is_attr_scrollable() -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
			err = hmi_windowscroll_weventrbar(&hmi-><%= w.struct_name() %>->wscr, wev, &dh, &dv);
			if (err < 0) {
				return;
			}
			if (err == 0) {
				<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_RSCROLLING);
			} else {
				<%= w.struct_name() %>_setflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_RSCROLLING);
			}
			<%- if w.is_exportevent_scroll() -%>
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_SCROLL;
			evt->data.<%= w.struct_name() %>_scroll.dh = dh;
			evt->data.<%= w.struct_name() %>_scroll.dv = dv;
			<%- end -%>
			return;
		}
		<%- end -%>
		<%- end -%>
		return;
	case	W_BBAR:
		<%- @win.each do |w| -%>
		<%- if w.is_attr_scrollable() -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
			err = hmi_windowscroll_weventbbar(&hmi-><%= w.struct_name() %>->wscr, wev, &dh, &dv);
			if (err < 0) {
				return;
			}
			if (err == 0) {
				<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_BSCROLLING);
			} else {
				<%= w.struct_name() %>_setflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_BSCROLLING);
			}
			<%- if w.is_exportevent_scroll() -%>
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_SCROLL;
			evt->data.<%= w.struct_name() %>_scroll.dh = dh;
			evt->data.<%= w.struct_name() %>_scroll.dv = dv;
			<%- end -%>
			return;
		}
		<%- end -%>
		<%- end -%>
		return;
	case	W_WORK:
		<%- @win.each do |w| -%>
		if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
			<%= w.struct_name() %>_butdnwork(hmi-><%= w.struct_name() %>, wev, evt);
			return;
		}
		<%- end -%>
		return;
	}

	return;
}

LOCAL VOID <%= self.main_name() %>_weventswitch(<%= self.main_name() %>_t *hmi, WEVENT *wev, <%= self.main_name() %>event_t *evt)
{
	<%= self.main_name() %>_setswitchbutdnflag(hmi);
	<%- @win.each do |w| -%>
	<%- if w.is_exportevent_switch() -%>
	if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
		evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_SWITCH;
		return;
	}
	<%- end -%>
	<%- end -%>
}

LOCAL VOID <%= self.main_name() %>_weventreswitch(<%= self.main_name() %>_t *hmi, WEVENT *wev, <%= self.main_name() %>event_t *evt)
{
	<%= self.main_name() %>_setswitchbutdnflag(hmi);
	<%- @win.each do |w| -%>
	if (<%= self.main_name() %>_is<%= w.struct_name() %>WID(hmi, wev->s.wid) == True) {
		<%- if w.is_exportevent_switch() -%>
		evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_SWITCH;
		<%= w.struct_name() %>_setflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_DRAWREQUEST);
		return;
		<%- else -%>
		<%- if w.is_exportevent_draw() -%>
		evt->type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_DRAW;
		<%- else -%>
		<%= w.struct_name() %>_redisp(hmi-><%= w.struct_name() %>);
		return;
		<%- end -%>
		<%- end -%>
	}
	<%- end -%>
}

LOCAL VOID <%= self.main_name() %>_receivemessage(<%= self.main_name() %>_t *hmi, <%= self.main_name() %>event_t *evt)
{
	MESSAGE msg;
	W err;

	err = rcv_msg(MM_ALL, &msg, sizeof(MESSAGE), WAIT|NOCLR);
	if (err >= 0) {
		if (msg.msg_type == MS_TMOUT) { /* should be use other type? */
			evt->type = <%= self.main_name().upcase %>EVENT_TYPE_COMMON_TIMEOUT;
			evt->data.common_timeout.code = msg.msg_body.TMOUT.code;
		}
	}
	clr_msg(MM_ALL, MM_ALL);
}

LOCAL Bool <%= self.main_name() %>_checkflag(<%= self.main_name() %>_t *hmi, <%= self.main_name() %>event_t **evt)
{
	W err, dh, dv;

	<%- @win.each do |w| -%>
	<%- if w.is_need_flag() and w.is_attr_scrollable() -%>
	if (<%= w.struct_name() %>_issetflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_RSCROLLING) == True) {
		err = hmi_windowscroll_weventrbar(&hmi-><%= w.struct_name() %>->wscr, &hmi->wev, &dh, &dv);
		if (err < 0) {
			<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_RSCROLLING);
			return False;
		}
		if (err == 0) {
			<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_RSCROLLING);
		} else {
			<%= w.struct_name() %>_setflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_RSCROLLING);
		}
		<%- if w.is_exportevent_scroll() -%>
		hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_SCROLL;
		hmi->evt.data.<%= w.struct_name() %>_scroll.dh = dh;
		hmi->evt.data.<%= w.struct_name() %>_scroll.dv = dv;
		<%- end -%>
		return True;
	}
	if (<%= w.struct_name() %>_issetflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_BSCROLLING) == True) {
		err = hmi_windowscroll_weventbbar(&hmi-><%= w.struct_name() %>->wscr, &hmi->wev, &dh, &dv);
		if (err < 0) {
			<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_BSCROLLING);
			return False;
		}
		if (err == 0) {
			<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_BSCROLLING);
		} else {
			<%= w.struct_name() %>_setflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_BSCROLLING);
		}
		<%- if w.is_exportevent_scroll() -%>
		hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_SCROLL;
		hmi->evt.data.<%= w.struct_name() %>_scroll.dh = dh;
		hmi->evt.data.<%= w.struct_name() %>_scroll.dv = dv;
		<%- end -%>
		return True;
	}
	<%- end -%>
	<%- end -%>
	<%- @win.each do |w| -%><%= w.generate_checkpartsflag_in_checkflag(self.main_name()) %><%- end -%>

	return False;
}

EXPORT W <%= self.main_name() %>_getevent(<%= self.main_name() %>_t *hmi, <%= self.main_name() %>event_t **evt)
{
	WEVENT	*wev0;
	Bool ok;

	hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_NONE;
	*evt = &hmi->evt;
	wev0 = &hmi->wev;

	ok = <%= self.main_name() %>_issetswitchbutdnflag(hmi);
	if (ok == True) {
		<%= self.main_name() %>_weventbutdn(hmi, wev0, &hmi->evt);
		<%= self.main_name() %>_clearswitchbutdnflag(hmi);
		return 0;
	}

	<%- @win.each do |w| -%>
	<%- if w.is_need_flag() -%>
	if (<%= w.struct_name() %>_issetflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_DRAWREQUEST) == True) {
		<%- if w.is_exportevent_draw() -%>
		hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_<%= w.struct_name().upcase %>_DRAW;
		<%- else -%>
		<%= w.struct_name() %>_redisp(hmi-><%= w.struct_name() %>);
		<%- end -%>
		<%= w.struct_name() %>_clearflag(hmi-><%= w.struct_name() %>, <%= w.struct_name().upcase %>_FLAG_DRAWREQUEST);
		return 0;
	}
	<%- end -%>
	<%- end -%>

	ok = <%= self.main_name() %>_checkflag(hmi, evt);
	if (ok == True) {
		return 0;
	}

	wget_evt(wev0, WAIT);
	switch (wev0->s.type) {
	case	EV_NULL:
		<%= self.main_name() %>_weventnull(hmi, wev0, &hmi->evt);
		break;
	case	EV_REQUEST:
		<%= self.main_name() %>_weventrequest(hmi, wev0, &hmi->evt);
		break;
	case	EV_RSWITCH:
		<%= self.main_name() %>_weventreswitch(hmi, wev0, &hmi->evt);
		break;
	case	EV_SWITCH:
		<%= self.main_name() %>_weventswitch(hmi, wev0, &hmi->evt);
		break;
	case	EV_BUTDWN:
		<%= self.main_name() %>_weventbutdn(hmi, wev0, &hmi->evt);
		break;
	case	EV_KEYDWN:
	case	EV_AUTKEY:
		hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_COMMON_KEYDOWN;
		hmi->evt.data.common_keydown.keycode = wev0->e.data.key.code;
		hmi->evt.data.common_keydown.keytop = wev0->e.data.key.keytop;
		hmi->evt.data.common_keydown.stat = wev0->e.stat;
		break;
	case	EV_INACT:
		pdsp_msg(NULL);
		break;
	case	EV_DEVICE:
		oprc_dev(&wev0->e, NULL, 0);
		break;
	case	EV_MSG:
		<%= self.main_name() %>_receivemessage(hmi, &hmi->evt);
		break;
	case	EV_MENU:
		hmi->evt.type = <%= self.main_name().upcase %>EVENT_TYPE_COMMON_MENU;
		hmi->evt.data.common_menu.pos = wev0->s.pos;
		break;
	}

	return 0;
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_struct()
    str = String.new();
    @win.each { |w|
      str += w.generate_source_struct();
    };
    script = <<-EOS
#define <%= self.main_name().upcase %>_FLAG_SWITCHBUTDN 0x00000001

struct <%= self.main_name() %>_t_ {
	WEVENT wev;
	<%= self.main_name() %>event_t evt;
	UW flag;
	<%- @win.each do |w| -%>
	<%= w.struct_name() %>_t *<%= w.struct_name() %>;
	<%- end -%>
};

    EOS
    erb = ERB.new(script, nil, '-');
    str += erb.result(binding)
  end

  def generate_windowfunctions()
    str = String.new();
    @win.each { |w|
      str += w.generate_source_functions(self.main_name());
    };
    str;
  end

  def generate_source_new_del_wrappers()
    script = <<-EOS
<%- @win.each do |w| -%>
EXPORT <%= w.struct_name() %>_t* <%= self.main_name() %>_new<%= w.struct_name() %>(<%= self.main_name() %>_t *hmi, <%= w.generate_initialize_arguments() %>)
{
	<%- if w.is_attr_subwindow -%>
	if (parent == NULL) {
		return NULL;
	}
	<%- end -%>
	if (hmi-><%= w.struct_name() %> != NULL) {
		return NULL;
	}
	hmi-><%= w.struct_name() %> = <%= w.struct_name() %>_new(<%= w.generate_initialize_arguments_values() %>);
	return hmi-><%= w.struct_name() %>;
}

EXPORT VOID <%= self.main_name() %>_delete<%= w.struct_name() %>(<%= self.main_name() %>_t *hmi, <%= w.struct_name() %>_t *window)
{
	<%= w.struct_name() %>_delete(hmi-><%= w.struct_name() %>);
	hmi-><%= w.struct_name() %> = NULL;
}

<%- end -%>

EXPORT <%= self.main_name() %>_t* <%= self.main_name() %>_new()
{
	<%= self.main_name() %>_t *hmi;

	hmi = (<%= self.main_name() %>_t *)malloc(sizeof(<%= self.main_name() %>_t));
	if (hmi == NULL) {
		return NULL;
	}
	hmi->flag = 0;
	<%- @win.each do |w| -%>
	hmi-><%= w.struct_name() %> = NULL;
	<%- end -%>

	return hmi;
}

EXPORT VOID <%= self.main_name() %>_delete(<%= self.main_name() %>_t *hmi)
{
	<%- @win.each do |w| -%>
	if (hmi-><%= w.struct_name() %> != NULL) {
		<%= w.struct_name() %>_delete(hmi-><%= w.struct_name() %>);
	}
	<%- end -%>
	free(hmi);
}
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_mainfunctions()
    str = String.new();
    @win.each { |w|
      str += w.generate_new_del_functions();
    };
    str += self.generate_source_new_del_wrappers()
    str;
  end

  def generate_source()
    str = String.new();
    str += self.generate_struct();
    str += self.generate_windowfunctions();
    str += self.generate_source_mainloop();
    str += self.generate_source_mainfunctions();
    str;
  end
end

#

def generate_header(filename, data)
  fd = File.open(filename, "w");
  fd.puts data.lisence_header();
  fd.puts <<-EOS

/* This file is automatically generated. */

#include    <basic.h>
#include	<btron/dp.h>
#include	<btron/hmi.h>

  EOS
  fd.puts "#ifndef __" + filename.upcase.gsub(/\./, '_') + "__";
  fd.puts "#define __" + filename.upcase.gsub(/\./, '_') + "__";
  fd.puts "\n";
  fd.puts data.generate_header();
  fd.puts "#endif\n"
end

def generate_source(filename, data)
  fd = File.open(filename, "w");
  fd.puts data.lisence_source();
  fd.puts <<-EOS

/* This file is automatically generated. */

  EOS
  fd.puts data.generate_source_include_files();

  fd.puts data.generate_source();

end

$KCODE = "EUC"

yaml = YAML.load_file(ARGV[0]);
data = HMIData.new(yaml);

fname_header = yaml["generator"]["output_header"]
fname_source = yaml["generator"]["output_source"]

generate_header(fname_header, data);
generate_source(fname_source, data);
