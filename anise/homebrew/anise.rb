class Anise < Formula
  desc "ANISE: A New Implemented Scripting Engine for ai5v"
  homepage "http://tomyun.pe.kr/projectanise/"
  url "http://kldp.net/download.php/1853/anisePlus-050118-src.tar.gz"
  version "0.11.20050118"
  sha256 "d21c04b62fcee4cd23f411a1e0ba65a98b1fe207dcc73fe67a1756f320b9fa13"

  depends_on "autoconf" => :build
  depends_on "automake" => :build
  depends_on "sdl"

  def install
    chmod 0755, "./autogen.sh"
    system "./autogen.sh"
    system "./configure", "--disable-dependency-tracking",
                          "--disable-silent-rules",
                          "--prefix=#{prefix}"
    system "make"
    bin.install "anisePlus" => "anise"
  end

  test do
    system "anise"
  end
end
