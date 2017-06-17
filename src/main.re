[@@@bs.config {no_export: no_export}];

module Html = Dom_html;

module Pg = Procedural_generator;

let loadCount = ref 0;

let imgsToLoad = 4;

let level_width = 2400.;

let level_height = 256.;

/*Canvas is chosen from the index.html file. The context is obtained from
 *the canvas. Listeners are added. A level is generated and the general
 *update_loop method is called to make the level playable.*/
let load _ => {
  Random.self_init ();
  let canvas_id = "canvas";
  let canvas =
    switch (Dom_html.getElementById Dom_html.document canvas_id) {
    | None =>
      Printf.printf "cant find canvas %s \n" canvas_id;
      failwith "fail"
    | Some el => Dom_html.elementToCanvasElement el
    };
  let context = (Dom_html.canvasElementToJsObj canvas)##getContext "2d";
  let _ = Dom_html.addEventListener Dom_html.document "keydown" Director.keydown Js.true_;
  let _ = Dom_html.addEventListener Dom_html.document "keyup" Director.keyup Js.true_;
  let () = Pg.init ();
  Director.update_loop
    canvas (Pg.generate level_width level_height context) (level_width, level_height)
};

let inc_counter _ => {
  loadCount := !loadCount + 1;
  if (!loadCount == imgsToLoad) {
    load ()
  } else {
    ()
  }
};

/*Used for concurrency issues.*/
let preload _ => {
  let root_dir = "sprites/";
  let imgs = ["blocks.png", "items.png", "enemies.png", "mario-small.png"];
  List.map
    (
      fun img_src => {
        let img_src = root_dir ^ img_src;
        let img = Html.createImg Dom_html.document;
        (Dom_html.imageElementToJsObj img)##src#=img_src;
        ignore (
          Dom_html.addEventListenerImg
            img
            "load"
            (
              fun _ev => {
                inc_counter ();
                Js.true_
              }
            )
            Js.true_
        )
      }
    )
    imgs
};

(Dom_html.windowToJsObj Dom_html.window)##onload#=(
                                                    fun _ => {
                                                      ignore (preload ());
                                                      Js.true_
                                                    }
                                                  );